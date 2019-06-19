#include <vlucur/vkall.h>
#include <vlucur/devices.h>
#include <vlucur/display.h>

static void set_values(struct vkcomp *app) {
  app->instance = VK_NULL_HANDLE;
  app->surface = VK_NULL_HANDLE;
  app->vk_layer_props = VK_NULL_HANDLE;
  app->vk_layer_count = VK_NULL_HANDLE;
  app->ep_instance_props = VK_NULL_HANDLE;
  app->ep_instance_count = VK_NULL_HANDLE;
  app->ep_device_props = VK_NULL_HANDLE;
  app->ep_device_count = VK_NULL_HANDLE;
  // app->device_properties;
  // app->device_features;
  // app->memory_properties;
  app->physical_device = VK_NULL_HANDLE;
  app->devices = VK_NULL_HANDLE;
  app->device_count = VK_NULL_HANDLE;
  app->queue_create_infos = VK_NULL_HANDLE;
  app->queue_families = VK_NULL_HANDLE;
  app->queue_family_count = 0;
  app->indices.graphics_family = -1;
  app->indices.present_family = -1;
  app->device = VK_FALSE;
  app->graphics_queue = VK_FALSE;
  // app->dets.capabilities = 0;
  app->dets.formats = VK_NULL_HANDLE;
  app->dets.format_count = VK_NULL_HANDLE;
  app->dets.present_modes = VK_NULL_HANDLE;
  app->dets.pres_mode_count = VK_NULL_HANDLE;
  app->swap_chain = VK_NULL_HANDLE;
  app->swap_chain_imgs = VK_NULL_HANDLE;
  // app->swap_chain_img_fmt = 0;
  // app->swap_chain_extent = 0;
  app->image_count = VK_NULL_HANDLE;
}

struct vkcomp *init_vk() {
  struct vkcomp *app;
  app = calloc(sizeof(struct vkcomp), sizeof(struct vkcomp));
  assert(app != NULL);
  set_values(app);
  return app;
}

/*
 * Gets all you're validation layers extensions
 * that comes installed with the vulkan sdk
 */
VkResult set_global_layers(struct vkcomp *app) {
  uint32_t layer_count = 0;
  VkLayerProperties *vk_props = NULL;
  VkResult res = VK_INCOMPLETE;

  do {
    res = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (res) return res;

    /* layer count will only be zero if vulkan sdk not installed */
    if (layer_count == 0) return VK_SUCCESS;

    vk_props = (VkLayerProperties *) realloc(vk_props, layer_count * sizeof(VkLayerProperties));
    if (!vk_props) return res;

    res = vkEnumerateInstanceLayerProperties(&layer_count, vk_props);
  } while (res == VK_INCOMPLETE);

  app->vk_layer_props = (VkLayerProperties *) \
    calloc(sizeof(VkLayerProperties), layer_count * sizeof(VkLayerProperties));
  if (!app->vk_layer_props) return res;

  fprintf(stdout, "\nVALIDATION LAYER SUPPORT:\n");
  /* Gather the extension list for each instance layer. */
  for (uint32_t i = 0; i < layer_count; i++) {
    res = get_extension_properties(NULL, &vk_props[i], NULL);
    if (res) return res;
    memcpy(&app->vk_layer_props[app->vk_layer_count], &vk_props[i], sizeof(vk_props[i]));
    fprintf(stdout, "%s\n", app->vk_layer_props[app->vk_layer_count].layerName);
    app->vk_layer_count = i;
  }

  free(vk_props);
  vk_props = NULL;

  return res;
}

/* Create connection between app and the vulkan api */
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name) {
  VkResult res = VK_INCOMPLETE;

  /* initialize the VkApplicationInfo structure */
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = engine_name;
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /* tells the Vulkan driver which instance extensions
    and global validation layers we want to use */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = NULL;
  create_info.enabledExtensionCount = 3;
  create_info.ppEnabledExtensionNames = instance_extensions;

  /* Create the instance */
  res = vkCreateInstance(&create_info, NULL, &app->instance);
  switch(res) {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return res;
    case VK_ERROR_INITIALIZATION_FAILED:
      return res;
    case VK_ERROR_LAYER_NOT_PRESENT:
      fprintf(stderr, "[x] one of the validation layers is currently not present");
      return res;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
    fprintf(stderr, "[x] one of the vulkan instance/device extensions is currently not present");
      return res;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      fprintf(stderr, "[x] cannot find a compatible Vulkan ICD\n");
      return res;
    default:
      fprintf(stderr, "[x] unknown error\n");
      return res;
  }

  res = get_extension_properties(app, NULL, NULL);
  if (res) return res;

  return res;
}

/* Get user physical device */
VkResult enumerate_devices(struct vkcomp *app) {
  VkResult res = VK_INCOMPLETE;
  res = vkEnumeratePhysicalDevices(app->instance, &app->device_count, NULL);
  if (res) return res;

  if (app->device_count == 0) {
    fprintf(stderr, "[x] failed to find GPUs with Vulkan support!");
    return res;
  }

  app->devices = (VkPhysicalDevice *) calloc(sizeof(VkPhysicalDevice),
      app->device_count * sizeof(VkPhysicalDevice));
  if (!app->devices) return res;

  res = vkEnumeratePhysicalDevices(app->instance, &app->device_count, app->devices);
  if (res) return res;

  /*
  * get a physical device that is suitable
  * to do the graphics related task that we need
  */
  for (uint32_t i = 0; i < app->device_count; i++) {
    if (is_device_suitable(app, app->devices[i]) &&
        find_queue_families(app, app->devices[i]) &&
        /* Check if current device has swap chain support */
        get_extension_properties(app, NULL, app->devices[i])) {
      app->physical_device = app->devices[i];
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    fprintf(stderr, "[x] failed to find a suitable GPU!");
    return res;
  }

  /* Query device properties */
  vkGetPhysicalDeviceMemoryProperties(app->physical_device, &app->memory_properties);
  vkGetPhysicalDeviceProperties(app->physical_device, &app->device_properties);
  fprintf(stdout, "\nPHYSICAL_DEVICE FOUND: %s\nMEMORY HEAP COUNT: %d\n",
    app->device_properties.deviceName, app->memory_properties.memoryHeapCount);

  return res;
}

/* After selecting a physical device to use.
  Set up a logical device to interface with it */
VkResult set_logical_device(struct vkcomp *app) {
  VkQueue present_queue;
  VkResult res = VK_INCOMPLETE;
  float queue_priorities[1] = {1.0};

  app->queue_create_infos = (VkDeviceQueueCreateInfo *) calloc(sizeof(VkDeviceQueueCreateInfo),
      app->queue_family_count * sizeof(VkDeviceQueueCreateInfo));
  if (!app->queue_create_infos) return res;

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    app->queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    app->queue_create_infos[i].pNext = NULL;
    app->queue_create_infos[i].flags = 0;
    app->queue_create_infos[i].queueFamilyIndex = i;
    app->queue_create_infos[i].queueCount = app->queue_family_count;
    app->queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
  create_info.pQueueCreateInfos = app->queue_create_infos;
  create_info.queueCreateInfoCount = app->queue_create_infos[0].queueCount;
  create_info.pEnabledFeatures = &app->device_features;
  create_info.enabledExtensionCount = 1;
  create_info.ppEnabledExtensionNames = device_extensions;
  create_info.ppEnabledLayerNames = NULL;
  create_info.enabledLayerCount = 0;

  /* Create logic device */
  res = vkCreateDevice(app->physical_device, &create_info, NULL, &app->device);
  if (res) return res;

  /*
   * Queues are automatically created with
   * the logical device, but you need a queue
   * handle to interface with them
   */
  vkGetDeviceQueue(app->device, app->indices.graphics_family, 0, &app->graphics_queue);
  vkGetDeviceQueue(app->device, app->indices.present_family, 0, &present_queue);

  return res;
}

VkResult create_swap_chain(struct vkcomp *app) {
  VkResult res = VK_INCOMPLETE;

  res = q_swapchain_support(app);
  if (res) return res;

  app->image_count = app->dets.capabilities.minImageCount + 1;

  /* Be sure image_count doesn't exceed the maximum. */
  if (app->dets.capabilities.maxImageCount > 0 && app->image_count > app->dets.capabilities.maxImageCount)
    app->image_count = app->dets.capabilities.maxImageCount;

  VkSurfaceFormatKHR surface_fmt = choose_swap_surface_format(app);
  VkPresentModeKHR pres_mode = chose_swap_present_mode(app);
  VkExtent2D extent = choose_swap_extent(app);

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = app->surface;
  create_info.minImageCount = app->image_count;
  create_info.imageFormat = surface_fmt.format;
  create_info.imageColorSpace = surface_fmt.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (app->indices.graphics_family != app->indices.present_family) {
    const uint32_t queue_family_indices[] = {
      app->indices.graphics_family,
      app->indices.present_family
    };
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;
  }

  /* specify that I currently want not transformation */
  create_info.preTransform = app->dets.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = pres_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  res = vkCreateSwapchainKHR(app->device, &create_info, NULL, &app->swap_chain);
  if (res) return res;

  res = vkGetSwapchainImagesKHR(app->device, app->swap_chain, &app->image_count, NULL);
  if (res) return res;

  app->swap_chain_imgs = (VkImage *) calloc(sizeof(VkImage), app->image_count * sizeof(VkImage));
  if (!app->swap_chain_imgs) return res;

  res = vkGetSwapchainImagesKHR(app->device, app->swap_chain, &app->image_count, app->swap_chain_imgs);
  if (res) return res;

  app->swap_chain_img_fmt = surface_fmt.format;
  app->swap_chain_extent = extent;

  return res;
}

VkResult create_img_views(struct vkcomp *app) {
  VkResult res = VK_INCOMPLETE;
  ALL_UNUSED(app);

  return res;
}

void freeup_vk(void *data) {
  struct vkcomp *app = (struct vkcomp *) data;
  if (app->swap_chain_imgs) {
    for (uint32_t i = 0; i < app->image_count; i++)
      vkDestroyImage(app->device, app->swap_chain_imgs[i], NULL);
    free(app->swap_chain_imgs);
  }
  // if (app->swap_chain)
  //   vkDestroySwapchainKHR(app->device, app->swap_chain, NULL);
  if (app->device) {
    vkDeviceWaitIdle(app->device);
    vkDestroyDevice(app->device, NULL);
  }
  if (app->vk_layer_props)
    free(app->vk_layer_props);
  if (app->ep_instance_props)
    free(app->ep_instance_props);
  if (app->ep_device_props)
    free(app->ep_device_props);
  if (app->devices)
    free(app->devices);
  if (app->queue_families)
    free(app->queue_families);
  if (app->queue_create_infos)
    free(app->queue_create_infos);
  if (app->dets.formats)
    free(app->dets.formats);
  if (app->dets.present_modes)
    free(app->dets.present_modes);
  if (app->surface)
    vkDestroySurfaceKHR(app->instance, app->surface, NULL);
  if (app->instance)
    vkDestroyInstance(app->instance, NULL);

  set_values(app);
  if (app)
    free(app);
  app = NULL;
}
