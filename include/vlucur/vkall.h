#ifndef VKALL_H
#define VKALL_H

#include <lucom.h>
#include <vulkan/vulkan_wayland.h>

/* Function protypes */
void initial_values(struct vkcomp *app);
VkResult check_validation_layer_support(struct vkcomp *app);
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name);
void cleanup(struct vkcomp *app);

#endif
