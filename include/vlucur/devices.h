#ifndef DEVICE_H
#define DEVICE_H

extern const char *device_extensions[1];
extern const char *instance_extensions[3];
extern const char *validation_extensions[17];

VkBool32 find_queue_families(struct vkcomp *app, VkPhysicalDevice device);
VkBool32 is_device_suitable(struct vkcomp *app, VkPhysicalDevice device);
VkResult get_extension_properties(struct vkcomp *app,
    VkLayerProperties *prop, VkPhysicalDevice device);

#endif
