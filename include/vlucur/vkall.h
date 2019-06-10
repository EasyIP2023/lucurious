#ifndef VKALL_H
#define VKALL_H

#include <lucom.h>
#include <vulkan/vulkan_wayland.h>

/* Function protypes */
void initial_values(struct vkcomp *app);
VkResult check_validation_layer_support(struct vkcomp *app);
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name);
VkResult enumerate_devices(struct vkcomp *app);
VkResult init_logical_device(struct vkcomp *app);
void cleanup(struct vkcomp *app);

#endif
