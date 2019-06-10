#include <vlucur/vkall.h>

VkResult vk_connect_surfaceKHR(struct vkcomp *app, VkWaylandSurfaceCreateInfoKHR create_info) {
  VkResult res = VK_FALSE;
  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  return res;
}
