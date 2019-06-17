#ifndef VKALL_H
#define VKALL_H

#include <lucom.h>

/* Function protypes */
struct vkcomp *init_vk();
VkResult set_global_layers(struct vkcomp *app);
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name);
VkResult enumerate_devices(struct vkcomp *app);
VkResult set_logical_device(struct vkcomp *app);
VkResult vk_connect_surfaceKHR(struct vkcomp *app, void *wl_display, void *wl_surface);
VkResult create_swap_chain(struct vkcomp *app);
void freeup_vk(void *data);

#endif
