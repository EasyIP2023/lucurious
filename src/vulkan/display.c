#include <lucom.h>
#include <vlucur/vkall.h>
#include <vlucur/display.h>

#define WIDTH 1920
#define HEIGHT 1080

VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
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

/* query device capabilities */
VkSurfaceCapabilitiesKHR q_device_capabilities(vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &capabilities);
  if (err) {
    fprintf(stderr, "[x] q_swapchain_capabilities: vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed, ERROR CODE: %d\n", err);
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  return capabilities;
}

/* choose swapchain surface format and color space (Color Depth) */
 VkSurfaceFormatKHR choose_swap_surface_format(vkcomp *app) {
  VkResult err;
  VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, NULL);
  if (err) {
    fprintf(stderr, "[x] choose_swap_surface_format: vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d\n", err);
    goto ret_format;
  }

  if (format_count == 0) {
    fprintf(stderr, "[x] choose_swap_surface_format: format_count equals 0\n");
    goto ret_format;
  }

  formats = (VkSurfaceFormatKHR *) calloc(sizeof(VkSurfaceFormatKHR),
    format_count * sizeof(VkSurfaceFormatKHR));

  if (!formats) {
    fprintf(stderr, "[x] choose_swap_surface_format: calloc VkSurfaceFormatKHR failed, ERROR CODE: %d\n", err);
    goto ret_format;
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, formats);
  if (err) {
    fprintf(stderr, "[x] choose_swap_surface_format: vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d\n", err);
    goto ret_format;
  }

  memcpy(&ret_fmt, &formats[0], sizeof(formats[0]));

  /*
   * VK_FORMAT_B8G8R8A8_UNORM till store the B, G, R and alpha channels
   * in that order with an 8 bit unsigned integer and a total of 32 bits per pixel.
   * SRGB if used for colorSpace if available, because it
   * results in more accurate perceived colors
   */
  if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
    ret_fmt.format = VK_FORMAT_B8G8R8A8_UNORM;
    ret_fmt.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    goto ret_format;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      memcpy(&ret_fmt, &formats[i], sizeof(formats[i]));
      goto ret_format;
    }
  }

ret_format:
  if (formats) {
    free(formats);
    formats = NULL;
  }
  return ret_fmt;
 }


/*
 * function that chooses the best presentation mode for swapchain
 * (Conditions required for swapping images to the screen)
 */
VkPresentModeKHR choose_swap_present_mode(vkcomp *app) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, NULL);
  if (err) {
    fprintf(stderr, "[x] choose_swap_present_mode: vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d\n", err);
    goto ret_best_mode;
  }

  if (pres_mode_count == 0) {
    fprintf(stderr, "[x] choose_swap_present_mode: pres_mode_count equals %d\n", pres_mode_count);
    goto ret_best_mode;
  }

  present_modes = (VkPresentModeKHR *) calloc(sizeof(VkPresentModeKHR),
      pres_mode_count * sizeof(VkPresentModeKHR));

  if (!present_modes) {
    fprintf(stderr, "[x] choose_swap_present_mode: VkPresentModeKHR calloc failed\n");
    goto ret_best_mode;
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, present_modes);
  if (err) {
    fprintf(stderr, "[x] choose_swap_present_mode: vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d\n", err);
    goto ret_best_mode;
  }

  /* Only mode that is guaranteed */
  best_mode = VK_PRESENT_MODE_FIFO_KHR;

  for (uint32_t i = 0; i < pres_mode_count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      memcpy(&best_mode, &present_modes[i], sizeof(present_modes[i]));
      /* For triple buffering */
      goto ret_best_mode;
    }
    else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      memcpy(&best_mode, &present_modes[i], sizeof(present_modes[i]));
    }
  }

ret_best_mode:
  if (present_modes) {
    free(present_modes);
    present_modes = NULL;
  }
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
