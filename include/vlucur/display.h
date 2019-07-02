#ifndef DISPLAY_H
#define DISPLAY_H

VkSurfaceCapabilitiesKHR q_device_capabilities(vkcomp *app);
VkSurfaceFormatKHR choose_swap_surface_format(vkcomp *app);
VkPresentModeKHR choose_swap_present_mode(vkcomp *app);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities);

#endif
