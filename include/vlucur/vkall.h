#ifndef VKALL_H
#define VKALL_H

#include <lucom.h>

/* Function protypes */
void initialize_vulkan_values(struct vkcomp *app);
VkResult check_validation_layer_support(struct vkcomp *app);
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name);
VkResult enumerate_devices(struct vkcomp *app);
VkResult init_logical_device(struct vkcomp *app);
VkResult vk_connect_surfaceKHR(struct vkcomp *app, VkWaylandSurfaceCreateInfoKHR create_info);
void freeup_vk(struct vkcomp *app);

#endif
