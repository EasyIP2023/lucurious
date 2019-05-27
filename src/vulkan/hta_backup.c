#include <vlucur/vlucur.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vulkan/vulkan.h>
#include <linux/input.h>
#include <cglm/call.h>

typedef struct hwayland {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_surface *window;
  struct wl_shell *shell;
  struct wl_shell_surface *shell_surface;
  struct wl_seat *seat;
  struct wl_pointer *pointer;
  struct wl_keyboard *keyboard;
  VkSurfaceKHR surface;
  bool prepared;
  bool use_staging_buffer;
  bool separate_present_queue;
  bool is_minimized;

  bool VK_KHR_incremental_present_enabled;

  bool VK_GOOGLE_display_timing_enabled;
  bool syncd_with_actual_presents;
  uint64_t refresh_duration;
  uint64_t refresh_duration_multiplier;
  uint64_t target_IPD;  // image present duration (inverse of frame rate)
  uint64_t prev_desired_present_time;
  uint32_t next_present_id;
  uint32_t last_early_id;  // 0 if no early images
  uint32_t last_late_id;   // 0 if no late images

  VkInstance inst;
  VkPhysicalDevice gpu;
  VkDevice device;
  VkQueue graphics_queue;
  VkQueue present_queue;
  uint32_t graphics_queue_family_index;
  uint32_t present_queue_family_index;
  VkSemaphore image_acquired_semaphores[FRAME_LAG];
  VkSemaphore draw_complete_semaphores[FRAME_LAG];
  VkSemaphore image_ownership_semaphores[FRAME_LAG];
  VkPhysicalDeviceProperties gpu_props;
  VkQueueFamilyProperties *queue_props;
  VkPhysicalDeviceMemoryProperties memory_properties;

  uint32_t enabled_extension_count;
  uint32_t enabled_layer_count;
  char *extension_names[64];
  char *enabled_layers[64];

  int width, height;
  VkFormat format;
  VkColorSpaceKHR color_space;

  PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
  PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
  PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
  PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
  PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
  PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
  PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
  PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
  PFN_vkQueuePresentKHR fpQueuePresentKHR;
  PFN_vkGetRefreshCycleDurationGOOGLE fpGetRefreshCycleDurationGOOGLE;
  PFN_vkGetPastPresentationTimingGOOGLE fpGetPastPresentationTimingGOOGLE;
  uint32_t swapchainImageCount;
  VkSwapchainKHR swapchain;
  SwapchainImageResources *swapchain_image_resources;
  VkPresentModeKHR presentMode;
  VkFence fences[FRAME_LAG];
  int frame_index;

  VkCommandPool cmd_pool;
  VkCommandPool present_cmd_pool;

  struct {
      VkFormat format;

      VkImage image;
      VkMemoryAllocateInfo mem_alloc;
      VkDeviceMemory mem;
      VkImageView view;
  } depth;

  struct texture_object textures[DEMO_TEXTURE_COUNT];
  struct texture_object staging_texture;

  VkCommandBuffer cmd;  // Buffer for initialization commands
  VkPipelineLayout pipeline_layout;
  VkDescriptorSetLayout desc_layout;
  VkPipelineCache pipelineCache;
  VkRenderPass render_pass;
  VkPipeline pipeline;

  mat4x4 projection_matrix;
  mat4x4 view_matrix;
  mat4x4 model_matrix;

  float spin_angle;
  float spin_increment;
  bool pause;

  VkShaderModule vert_shader_module;
  VkShaderModule frag_shader_module;

  VkDescriptorPool desc_pool;

  bool quit;
  int32_t curFrame;
  int32_t frameCount;
  bool validate;
  bool validate_checks_disabled;
  bool use_break;
  bool suppress_popups;

  PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
  PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
  PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;
  PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
  PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
  PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
  PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
  VkDebugUtilsMessengerEXT dbg_messenger;

  uint32_t current_buffer;
  uint32_t queue_family_count;
} hwayland;

/* Connection between application and the Vulkan library */
VkInstance instance;
VkExtensionProperties *vkprops;

static void demo_run(struct demo *demo) {
    while (!demo->quit) {
        if (demo->pause) {
            wl_display_dispatch(demo->display);  // block and wait for input
        } else {
            wl_display_dispatch_pending(demo->display);  // don't block
            demo_draw(demo);
            demo->curFrame++;
            if (demo->frameCount != INT32_MAX && demo->curFrame == demo->frameCount) demo->quit = true;
        }
    }
}

static void handle_ping(void *data UNUSED, struct wl_shell_surface *shell_surface, uint32_t serial) {
    wl_shell_surface_pong(shell_surface, serial);
}

static void handle_configure(void *data UNUSED, struct wl_shell_surface *shell_surface UNUSED, uint32_t edges UNUSED,
                             int32_t width UNUSED, int32_t height UNUSED) {}

static void handle_popup_done(void *data UNUSED, struct wl_shell_surface *shell_surface UNUSED) {}

static const struct wl_shell_surface_listener shell_surface_listener = {handle_ping, handle_configure, handle_popup_done};

static void create_window(struct demo *demo) {
    demo->window = wl_compositor_create_surface(demo->compositor);
    if (!demo->window) {
        printf("Can not create wayland_surface from compositor!\n");
        fflush(stdout);
        exit(1);
    }

    demo->shell_surface = wl_shell_get_shell_surface(demo->shell, demo->window);
    if (!demo->shell_surface) {
        printf("Can not get shell_surface from wayland_surface!\n");
        fflush(stdout);
        exit(1);
    }
    wl_shell_surface_add_listener(demo->shell_surface, &shell_surface_listener, demo);
    wl_shell_surface_set_toplevel(demo->shell_surface);
    wl_shell_surface_set_title(demo->shell_surface, APP_SHORT_NAME);
}

void main_loop() {
  printf("glfwWindow %d\n", glfwWindowShouldClose(window));

  // Still working on why window not showing up
  // Could be the wayland compositor I'm using
  return;

}

void create_instance() {
  VkResult err;

  VkApplicationInfo app_info = {};
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /* tells the Vulkan driver which global extensions
    and validation layers we want to use */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  // create_info.enabledExtensionCount = glfw_extension_count;
  // create_info.ppEnabledExtensionNames = glfw_extensions;
  create_info.enabledLayerCount = 0;

  /* Create the instance */
  VkResult result = vkCreateInstance(&create_info, NULL, &instance);
  if (result != VK_SUCCESS) {
    perror("[x] failed to created instance");
    return;
  }


  uint32_t extension_count = 0;
  err = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
  assert(!err);

  if (extension_count > 0) {
    vkprops = calloc(sizeof(VkExtensionProperties), extension_count * sizeof(VkExtensionProperties));
    assert(vkprops != NULL);

    err = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, vkprops);
    assert(!err);

    printf("Instance created\navailable extesions: %d\n",extension_count);

    for (unsigned int i = 0; i < extension_count; i++) {
      printf("%s\n", vkprops[i].extensionName);

      if (!strcmp(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, vkprops[i].extensionName)) {
        platformSurfaceExtFound = 1;
        demo->extension_names[demo->enabled_extension_count++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
      }
    }
  }
}

void init_vk_swapchain(struct demo *demo) {
  VkResult U_ASSERT_ONLY err;
  VkWaylandSurfaceCreateInfoKHR createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.display = demo->display;
  createInfo.surface = demo->window;

  err = vkCreateWaylandSurfaceKHR(demo->inst, &createInfo, NULL, &demo->surface);
  assert(!err);
}

static void pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx,
                                 wl_fixed_t sy) {}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {}

static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button,
                                  uint32_t state) {
    struct demo *demo = data;
    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        wl_shell_surface_move(demo->shell_surface, demo->seat, serial);
    }
}

static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {}

static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter, pointer_handle_leave, pointer_handle_motion, pointer_handle_button, pointer_handle_axis,
};

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size) {}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys) {}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state) {
    if (state != WL_KEYBOARD_KEY_STATE_RELEASED) return;
    struct demo *demo = data;
    switch (key) {
        case KEY_ESC:  // Escape
            demo->quit = true;
            break;
        case KEY_LEFT:  // left arrow key
            demo->spin_angle -= demo->spin_increment;
            break;
        case KEY_RIGHT:  // right arrow key
            demo->spin_angle += demo->spin_increment;
            break;
        case KEY_SPACE:  // space bar
            demo->pause = !demo->pause;
            break;
    }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {}

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap, keyboard_handle_enter, keyboard_handle_leave, keyboard_handle_key, keyboard_handle_modifiers,
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat, enum wl_seat_capability caps) {
    // Subscribe to pointer events
    struct demo *demo = data;
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !demo->pointer) {
        demo->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(demo->pointer, &pointer_listener, demo);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && demo->pointer) {
        wl_pointer_destroy(demo->pointer);
        demo->pointer = NULL;
    }
    // Subscribe to keyboard events
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        demo->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(demo->keyboard, &keyboard_listener, demo);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
        wl_keyboard_destroy(demo->keyboard);
        demo->keyboard = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
};

static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
                                   uint32_t version UNUSED) {
    struct demo *demo = data;
    // pickup wayland objects when they appear
    if (strcmp(interface, "wl_compositor") == 0) {
        demo->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        demo->shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        demo->seat = wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(demo->seat, &seat_listener, demo);
    }
}

static void registry_handle_global_remove(void *data UNUSED, struct wl_registry *registry UNUSED, uint32_t name UNUSED) {}

static const struct wl_registry_listener registry_listener = {registry_handle_global, registry_handle_global_remove};

void init_connection(hwayland *lucur) {
  demo->display = wl_display_connect(NULL);

  if (demo->display == NULL) {
      printf("Cannot find a compatible Vulkan installable client driver (ICD).\nExiting ...\n");
      fflush(stdout);
      exit(1);
  }

  demo->registry = wl_display_get_registry(demo->display);
  wl_registry_add_listener(demo->registry, &registry_listener, demo);
  wl_display_dispatch(demo->display);
}

void init_vulkan() {
  create_instance();
}

void cleanup(hwayland *lucur) {
  free(vkprops);
  vkprops = NULL;
  vkDestroyInstance(instance, NULL);

  wl_keyboard_destroy(lucur->keyboard);
  wl_pointer_destroy(lucur->pointer);
  wl_seat_destroy(lucur->seat);
  wl_shell_surface_destroy(lucur->shell_surface);
  wl_surface_destroy(lucur->window);
  wl_shell_destroy(lucur->shell);
  wl_compositor_destroy(lucur->compositor);
  wl_registry_destroy(lucur->registry);
  wl_display_disconnect(lucur->display);
}

void run() {
  hwayland lucur;
  create_window(&lucur);
  init_vk_swapchain(&lucur);
  prepare(&lucur);
  run(&lucur);
  cleanup(&lucur);
}
