#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>

/* How wayland display's and surface's connect to your vulkan application */
VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->instance) {
    wlu_log_me(WLU_DANGER, "[x] A VkInstance must be established");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_instance(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

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
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &capabilities);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed, ERROR CODE: %d", err);
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  return capabilities;
}

/* choose swapchain surface format and color space (Color Depth) */
VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace) {
  VkResult err;
  VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_format;
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, NULL);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d", err);
    goto finish_format;
  }

  if (format_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device surface formats, format_count equals 0");
    goto finish_format;
  }

  formats = (VkSurfaceFormatKHR *) calloc(sizeof(VkSurfaceFormatKHR),
    format_count * sizeof(VkSurfaceFormatKHR));
  if (!formats) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkSurfaceFormatKHR *formats failed");
    goto finish_format;
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, formats);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d", err);
    goto finish_format;
  }

  ret_fmt = formats[0];

  /* The format in the example
   * VK_FORMAT_B8G8R8A8_UNORM will store the B, G, R and alpha channels
   * in that order with an 8 bit unsigned integer and a total of 32 bits per pixel.
   * SRGB if used for colorSpace if available, because it
   * results in more accurate perceived colors
   */
  if (format_count == 1 && formats[0].format == format) {
    ret_fmt.format = format;
    ret_fmt.colorSpace = colorSpace;
    goto finish_format;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == format && formats[i].colorSpace == colorSpace) {
      ret_fmt = formats[i];
      goto finish_format;
    }
  }

finish_format:
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
VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_best_mode;
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, NULL);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d", err);
    goto finish_best_mode;
  }

  if (pres_mode_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device presentation modes, pres_mode_count equals 0");
    goto finish_best_mode;
  }

  present_modes = (VkPresentModeKHR *) calloc(sizeof(VkPresentModeKHR),
      pres_mode_count * sizeof(VkPresentModeKHR));

  if (!present_modes) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkPresentModeKHR *present_modes failed");
    goto finish_best_mode;
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, present_modes);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d", err);
    goto finish_best_mode;
  }

  /* Only mode that is guaranteed */
  best_mode = VK_PRESENT_MODE_FIFO_KHR;

  for (uint32_t i = 0; i < pres_mode_count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      best_mode = present_modes[i];
      /* For triple buffering */
      goto finish_best_mode;
    }
    else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      best_mode = present_modes[i];
    }
  }

finish_best_mode:
  if (present_modes) {
    free(present_modes);
    present_modes = NULL;
  }
  return best_mode;
}

/* The swap extent is the resolution of the swap chain images */
VkExtent2D wlu_choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = {width, height};

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
