#ifndef DEVICE_H
#define DEVICE_H

VkBool32 set_queue_family(vkcomp *app,
    VkPhysicalDevice device, VkQueueFlagBits vkqfbits);

VkBool32 is_device_suitable(vkcomp *app,
    VkPhysicalDevice device, VkPhysicalDeviceType vkpdtype);

VkResult get_extension_properties(vkcomp *app,
    VkLayerProperties *prop, VkPhysicalDevice device);

#endif
