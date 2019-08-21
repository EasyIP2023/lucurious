/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKALL_H
#define VKALL_H

#include <cglm/call.h>

#define VK_USE_PLATFORM_WAYLAND_KHR 1
#include <vulkan/vulkan.h>

typedef struct swap_chain_buffers {
  VkImage image;
  VkImageView view;
} swap_chain_buffers;

typedef struct queue_family_indices {
  uint32_t graphics_family;
  uint32_t present_family;
} queue_family_indices;

typedef struct vkcomp {
  PFN_vkCreateDebugReportCallbackEXT dbg_create_report_callback;
  PFN_vkDestroyDebugReportCallbackEXT dbg_destroy_report_callback;
  PFN_vkDebugReportMessageEXT debug_messenger;
  uint32_t dbg_size;
  VkDebugReportCallbackEXT *debug_report_callbacks;

  VkInstance instance;
  VkSurfaceKHR surface;

  /* keep track of all vulkan extensions */
  VkLayerProperties *vk_layer_props;
  uint32_t vk_layer_count;

  VkExtensionProperties *ep_instance_props;
  uint32_t ep_instance_count;

  VkExtensionProperties *ep_device_props;
  uint32_t ep_device_count;

  /* To get device properties like the name, type and supported Vulkan version */
  VkPhysicalDeviceProperties device_properties;
  /* For optional features like texture compression,
    64 bit floats and multi viewport rendering */
  VkPhysicalDeviceFeatures device_features;
  VkPhysicalDeviceMemoryProperties memory_properties;
  VkPhysicalDevice physical_device;

  VkDeviceQueueCreateInfo *queue_create_infos;
  VkQueueFamilyProperties *queue_families;
  uint32_t queue_family_count;
  queue_family_indices indices;

  VkDevice device; /* logical device */
  VkQueue graphics_queue;
  VkQueue present_queue;

  swap_chain_buffers *sc_buffs;
  VkSwapchainKHR swap_chain;
  uint32_t sc_buff_size;

  VkRenderPass render_pass;
  VkPipelineLayout pipeline_layout;
  VkPipeline graphics_pipeline;

  VkFramebuffer *sc_frame_buffs;

  VkCommandPool cmd_pool;
  VkCommandBuffer *cmd_buffs;

  VkSemaphore img_semaphore;
  VkSemaphore render_semaphore;

  struct {
    VkFormat format;
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
  } depth;

  /* Start of uniform buffer section */
  struct {
    VkBuffer buff;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo buff_info;
  } uniform_data;

  mat4 proj;
  mat4 view;
  mat4 model;
  mat4 clip;
  mat4 mvp;
  /* End of uniform buffer section */

  uint32_t desc_count;
  VkDescriptorSetLayout *desc_layout;
  VkDescriptorPool desc_pool;
  VkDescriptorSet *desc_set;
} vkcomp;

vkcomp *wlu_init_vk();

/* Set vulkan validation layers properties.
 * To get these validation layers you must install
 * vulkan sdk
 */
VkResult wlu_set_global_layers(vkcomp *app);

VkResult wlu_set_debug_message(vkcomp *app, uint32_t size);

/* Create connection between app and the vulkan api */
VkResult wlu_create_instance(
  vkcomp *app,
  char *app_name,
  char *engine_name,
  uint32_t enabledLayerCount,
  const char* const* ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char* const* ppEnabledExtensionNames
);

/*
 * This function will select the physical device of
 * your choosing based off of VkPhysicalDeviceType
 */
VkResult wlu_enumerate_devices(vkcomp *app, VkPhysicalDeviceType vkpdtype);

/*
 * Almost every operation in Vulkan, from submitting command buffers
 * to presenting images to a surface, requires commands to be submitted
 * to a hardware queue. This will create multiple queue families
 * that are supported by the VkQueueFlagBits set and assign the
 * available graphics and present queues
 */
VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits);

/*
 * After selecting a physical device to use.
 * Set up a logical device to interface with your physical device
 * This function is also used to set Vulkan Device Level Extensions
 * that entail what a device does
 */
VkResult wlu_create_logical_device(
  vkcomp *app,
  uint32_t enabledLayerCount,
  const char* const* ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char* const* ppEnabledExtensionNames
);

/* How wayland display's and surface's connect to your vulkan application */
VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface);

/*
 * Needed to create the swap chain. This function queries your physical device's
 * capabilities. Mainly used to get minImageCount and the extent/resolution
 * that a particular physical device
 */
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app);

/*
 * Needed to create the swap chain. This will specify the format and
 * the surace of an image. The "format" variable refers to the pixel
 * formats and the "colorSpace" variable refers to the Color Depth
 */
VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace);

/*
 * Needed to create the swap chain
 * This function chooses the best presentation mode for swapchain
 * (Conditions required for swapping images to the screen)
 */
VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app);

/*
 * Needed to create the swap chain. Pick the resolution
 * for the images in the swap chain
 */
VkExtent2D wlu_choose_2D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height);

/*
 * Needed to create the swap chain. Pick the resolution
 * for the images in the swap chain along with the depth
 * of the 3D object
 */
VkExtent3D wlu_choose_3D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height, uint32_t depth);

/* Create the actual swap chain used to present images to a surface */
VkResult wlu_create_swap_chain(
  vkcomp *app,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR pres_mode,
  VkExtent2D extent,
  VkExtent3D extent3D
);

void wlu_retrieve_device_queue(vkcomp *app);

/*
 * Create image views which is the way you communicate to vulkan
 * on how you intend to use the images in the swap chain
 */
VkResult wlu_create_img_views(vkcomp *app, VkFormat format, VkImageViewType type);

/* Need to depth buffer to render 3D images (only need one) */
VkResult wlu_create_depth_buff(
  vkcomp *app,
  VkFormat depth_format,
  VkFormatFeatureFlags linearTilingFeatures,
  VkFormatFeatureFlags optimalTilingFeatures,
  VkImageType imageType,
  VkExtent3D extent,
  VkImageUsageFlags usage,
  VkSharingMode sharingMode,
  VkImageLayout initialLayout,
  VkImageViewType viewType
);

/*
 * Function creates a uniform buffer so that shaders can access
 * in a read-only fashion constant parameter data.
 */
VkResult wlu_create_uniform_buff(vkcomp *app, VkBufferCreateFlagBits flags, VkBufferUsageFlags usage);

VkResult wlu_create_framebuffers(vkcomp *app, uint32_t attachment_count, VkExtent2D extent, uint32_t layers);

/*
 * Allows for your app to create a command pool to store your
 * command buffers before being committed to main memory
 */
VkResult wlu_create_cmd_pool(vkcomp *app, VkCommandPoolCreateFlagBits flags);

/* Allows for your app to submmit graphics commands to render and image */
VkResult wlu_create_cmd_buffs(vkcomp *app, VkCommandBufferLevel level);

VkResult wlu_exec_begin_cmd_buff(
  vkcomp *app,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
);

VkResult wlu_exec_stop_cmd_buff(vkcomp *app);

/* Acquire the swapchain image in order to set its layout */
VkResult wlu_retrieve_swapchain_img(vkcomp *app, uint32_t *current_buffer);

VkResult wlu_draw_frame(
  vkcomp *app,
  uint32_t *image_index,
  uint32_t waitSemaphoreCount,
  VkSemaphore *pWaitSemaphores,
  VkPipelineStageFlags *pWaitDstStageMask,
  uint32_t commandBufferCount,
  uint32_t signalSemaphoreCount,
  VkSemaphore *pSignalSemaphores,
  uint32_t swapchainCount,
  VkSwapchainKHR *pSwapchains,
  VkResult *pResults
);

/*
 * Can find in Vulkan SDK samples/API-Samples/10-init_render_pass
 * A semaphore (or fence) is required in order to acquire a
 * swapchain image to prepare it for use in a render pass.
 * The semaphore is normally used to hold back the rendering
 * operation until the image is actually available. This function
 * creates semaphores
 */
VkResult wlu_create_semaphores(vkcomp *app);

void wlu_freeup_vk(void *data);

#endif
