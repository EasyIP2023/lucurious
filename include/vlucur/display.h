#ifndef DISPLAY_H
#define DISPLAY_H

VkResult q_swapchain_support(struct vkcomp *app, VkPhysicalDevice device);
VkSurfaceFormatKHR choose_swap_surface_format(struct vkcomp *app);
VkPresentModeKHR chose_swap_present_mode(struct vkcomp *app);
VkExtent2D choose_swap_extent(struct vkcomp *app);

#endif
