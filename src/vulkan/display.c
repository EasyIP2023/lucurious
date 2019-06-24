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
  create_info.display = wl_display;
  create_info.surface = wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  return res;
}

/* query swap chain details */
VkResult q_swapchain_support(struct vkcomp *app) {
  VkResult res = VK_INCOMPLETE;

  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &app->dets.capabilities);
  if (res) return res;

  res = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &app->dets.format_count, NULL);
  if (res) return res;

  if (app->dets.format_count != 0) {
    app->dets.formats = (VkSurfaceFormatKHR * ) \
      calloc(sizeof(VkSurfaceFormatKHR), app->dets.format_count * sizeof(VkSurfaceFormatKHR));
    if (!app->dets.formats) return 0;

    res = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &app->dets.format_count, app->dets.formats);
    if (res) return res;
  } else {
    return VK_INCOMPLETE;
  }

  res = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &app->dets.pres_mode_count, NULL);
  if (res) return res;

  if (app->dets.pres_mode_count != 0) {
    app->dets.present_modes = (VkPresentModeKHR *) \
      calloc(sizeof(VkPresentModeKHR), app->dets.pres_mode_count * sizeof(VkPresentModeKHR));
    if (!app->dets.formats) return res;

    res = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &app->dets.pres_mode_count, app->dets.present_modes);
    if (res) return res;
  } else {
    return VK_INCOMPLETE;
  }

  return res;
}

/*
 * function that choose swapchain surface format (Color Depth)
 * For more info go here
 * https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
 */
VkSurfaceFormatKHR choose_swap_surface_format(struct vkcomp *app) {
  if (app->dets.format_count == 1 && app->dets.formats[0].format == VK_FORMAT_UNDEFINED) {
    VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    return ret_fmt;
  }

  for (uint32_t i = 0; i < app->dets.format_count; i++) {
    if (app->dets.formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
        app->dets.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        return app->dets.formats[i];
  }

  return app->dets.formats[0];
}

/*
 * function that chooses the best presentation mode for swapchain
 * (Conditions required for swapping images to the screen)
 * For more info go here
 * https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
 */
VkPresentModeKHR choose_swap_present_mode(struct vkcomp *app) {
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR; /* Only mode that is guaranteed */

  for (uint32_t i = 0; i < app->dets.pres_mode_count; i++) {
    if (app->dets.present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return app->dets.present_modes[i]; /* For triple buffering */
    }
    else if (app->dets.present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
      best_mode = app->dets.present_modes[i];
  }

  return best_mode;
}

/* The swap extent is the resolution of the swap chain images */
VkExtent2D choose_swap_extent(struct vkcomp *app) {
  if (app->dets.capabilities.currentExtent.width != UINT32_MAX) {
    return app->dets.capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = {WIDTH, HEIGHT};

    actual_extent.width = max(app->dets.capabilities.minImageExtent.width,
                          min(app->dets.capabilities.maxImageExtent.width,
                          actual_extent.width));
    actual_extent.height = max(app->dets.capabilities.minImageExtent.height,
                           min(app->dets.capabilities.maxImageExtent.height,
                           actual_extent.height));

    /* resolution will most likely result in 1080p or 1920x1080 */
    return actual_extent;
  }
}
