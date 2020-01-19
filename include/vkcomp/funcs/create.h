/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#ifndef WLU_VKCOMP_CREATE_FUNCS_H
#define WLU_VKCOMP_CREATE_FUNCS_H

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

/**
* This function will select the physical device of
* your choosing based off of VkPhysicalDeviceType
*/
VkResult wlu_create_physical_device(
  vkcomp *app,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_properties,
  VkPhysicalDeviceFeatures *device_features
);

/**
* After selecting a physical device to use.
* Set up a logical device to interface with your physical device
* This function is also used to set Vulkan Device Level Extensions
* that entail what a device does
*/
VkResult wlu_create_logical_device(
  vkcomp *app,
  VkPhysicalDeviceFeatures *device_feats,
  uint32_t queue_count,
  uint32_t enabledLayerCount,
  const char *const *ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
);

/* Create the actual swap chain used to present images to a surface */
VkResult wlu_create_swap_chain(
  vkcomp *app,
  uint32_t cur_scd,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR pres_mode,
  uint32_t width,
  uint32_t height
);

/**
* Create image views which is the way you communicate to vulkan
* on how you intend to use the images in a swap chain
*/
VkResult wlu_create_img_views(
  vkcomp *app,
  uint32_t cur_scd,
  VkFormat format,
  VkImageViewType type
);

/* Need to depth buffer to render 3D images (only need one) */
VkResult wlu_create_depth_buff(
  vkcomp *app,
  uint32_t cur_scd,
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

/**
* Function creates buffers like a uniform buffer so that shaders can access
* in a read-only fashion constant parameter data. Function also
* creates buffers like a vertex buffer so that it's visible to the CPU
*/
VkResult wlu_create_buffer(
  vkcomp *app,
  uint32_t cur_bd,
  VkDeviceSize size,
  VkBufferCreateFlagBits flags,
  VkBufferUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  char *buff_name,
  VkFlags requirements_mask
);

VkResult wlu_create_buff_mem_map(
  vkcomp *app,
  uint32_t cur_bd,
  void *data
);

VkResult wlu_create_framebuffers(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  VkImageView *attachments,
  uint32_t width,
  uint32_t height,
  uint32_t layers
);

/**
* Allows for your app to create a command pool to store your
* command buffers before being committed to main memory
*/
VkResult wlu_create_cmd_pool(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t cur_cmdd,
  uint32_t queueFamilyIndex,
  VkCommandPoolCreateFlagBits flags
);

/**
* Allows for your app to put commands into a buffer to later
* be submitted to one of the hardware queues
*/
VkResult wlu_create_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferLevel level
);

/**
* Can find in Vulkan SDK samples/API-Samples/10-init_render_pass
* A semaphore (or fence) is required in order to acquire a
* swapchain image to prepare it for use in a render pass.
* The semaphore is normally used to hold back the rendering
* operation until the image is actually available. This function
* creates semaphores
*/
VkResult wlu_create_semaphores(vkcomp *app, uint32_t cur_scd);

#endif
