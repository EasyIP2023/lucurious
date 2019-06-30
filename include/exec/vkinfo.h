#ifndef VKINFO_H
#define VKINFO_H

void help_message();
void version_num();
void print_gvalidation_layers();
void print_instance_extensions();
void print_device_extensions(VkQueueFlagBits vkqfbits,
                             VkPhysicalDeviceType dt);

#endif
