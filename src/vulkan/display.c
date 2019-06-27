#include <lucom.h>
#include <vlucur/vkall.h>
#include <vlucur/display.h>

#define WIDTH 1920
#define HEIGHT 1080

VkResult wlu_vkconnect_surfaceKHR(struct vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_INCOMPLETE;
  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = (struct wl_display *) wl_display;
  create_info.surface = (struct wl_surface *) wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  return res;
}

/* query swap chain details */
VkSurfaceCapabilitiesKHR q_swapchain_capabilities(struct vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &capabilities);
  if (err) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] q_swapchain_capabilities: vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed\n");
    exit(-1);
  }

  return capabilities;
}

/*
 * function that choose swapchain surface format (Color Depth)
 * For more info go here
 * https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
 */
 VkSurfaceFormatKHR choose_swap_surface_format(struct vkcomp *app) {
  VkResult err;
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, NULL);
  if (err) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_surface_format: vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d\n", err);
    exit(-1);
  }

  if (format_count == 0) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_surface_format: format_count equals 0\n");
    exit(-1);
  }

  formats = (VkSurfaceFormatKHR *) calloc(sizeof(VkSurfaceFormatKHR),
    format_count * sizeof(VkSurfaceFormatKHR));

  if (!formats) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_surface_format: calloc VkSurfaceFormatKHR failed, ERROR CODE: %d\n", err);
    exit(-1);
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, formats);
  if (err) {
    free(formats);
    formats = NULL;
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_surface_format: vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d\n", err);
    exit(-1);
  }

  VkSurfaceFormatKHR ret_fmt;
  memcpy(&ret_fmt, &formats[0], sizeof(formats[0]));

  if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
    free(formats);
    formats = NULL;
    VkSurfaceFormatKHR rf = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    return rf;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      memcpy(&ret_fmt, &formats[i], sizeof(formats[i]));
      free(formats);
      formats = NULL;
      return ret_fmt;
    }
  }

  free(formats);
  formats = NULL;

  return ret_fmt;
 }


/*
 * function that chooses the best presentation mode for swapchain
 * (Conditions required for swapping images to the screen)
 * For more info go here
 * https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
 */
VkPresentModeKHR choose_swap_present_mode(struct vkcomp *app) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR; /* Only mode that is guaranteed */
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, NULL);
  if (err) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_present_mode: vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d\n", err);
    exit(-1);
  }

  if (pres_mode_count == 0) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_present_mode: pres_mode_count equals 0\n");
    exit(-1);
  }

  present_modes = (VkPresentModeKHR *) calloc(sizeof(VkPresentModeKHR),
      pres_mode_count * sizeof(VkPresentModeKHR));
  if (!present_modes) {
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_present_mode: VkPresentModeKHR calloc failed\n");
    exit(-1);
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, present_modes);
  if (err) {
    free(present_modes);
    present_modes = NULL;
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] choose_swap_present_mode: vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d\n", err);
    exit(-1);
  }

  for (uint32_t i = 0; i < pres_mode_count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      memcpy(&best_mode, &present_modes[i], sizeof(present_modes[i]));
      free(present_modes);
      present_modes = NULL;
      return best_mode; /* For triple buffering */
    }
    else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      memcpy(&best_mode, &present_modes[i], sizeof(present_modes[i]));
    }
  }

  free(present_modes);
  present_modes = NULL;

  return best_mode;
}

/* The swap extent is the resolution of the swap chain images */
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = {WIDTH, HEIGHT};

    actual_extent.width = max(capabilities.minImageExtent.width,
                          min(capabilities.maxImageExtent.width,
                          actual_extent.width));
    actual_extent.height = max(capabilities.minImageExtent.height,
                           min(capabilities.maxImageExtent.height,
                           actual_extent.height));

    /* resolution will most likely result in 1080p or 1920x1080 */
    return actual_extent;
  }
}
