#ifndef DEVICE_H
#define DEVICE_H

extern const char *device_extensions[];
extern const char *instance_extensions[];
extern const char *enabled_validation_layers[];

VkBool32 find_queue_families(vkcomp *app,
    VkPhysicalDevice device, VkQueueFlagBits vkqfbits);

VkBool32 is_device_suitable(vkcomp *app,
    VkPhysicalDevice device, VkPhysicalDeviceType vkpdtype);

VkResult get_extension_properties(vkcomp *app,
    VkLayerProperties *prop, VkPhysicalDevice device);

#endif
