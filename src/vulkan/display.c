#include <vlucur/vkall.h>

VkResult vk_connect_surfaceKHR(struct vkcomp *app, void *wl_display, void *wl_surface) {
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
