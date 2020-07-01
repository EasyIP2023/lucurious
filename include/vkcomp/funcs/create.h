/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2020 Vincent Davis Jr.
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

#ifndef DLU_VKCOMP_CREATE_FUNCS_H
#define DLU_VKCOMP_CREATE_FUNCS_H

/* Create connection between app and the vulkan api */
VkResult dlu_create_instance(
  vkcomp *app,
  char *app_name,
  char *engine_name,
  uint32_t enabledLayerCount,
  const char **ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char **ppEnabledExtensionNames
);

/**
* How Vulkan establishes connection with window system.
* Through the use of Window System Integration (WSI).
* This fuction exposes a VkSurfaceKHR object. This object
* represents a surface to present rendered images to.
*/
VkResult dlu_create_vkwayland_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface);

/**
* This function will select the physical device of
* your choosing based off of VkPhysicalDeviceType
*/
VkResult dlu_create_physical_device(
  vkcomp *app,
  uint32_t cur_pd,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
);

/**
* Almost every operation in Vulkan, from submitting command buffers
* to presenting images to a surface, requires commands to be submitted
* to a hardware queue. This will create multiple queue family indices
* that are supported by a device.
*/
VkBool32 dlu_create_queue_families(vkcomp *app, uint32_t cur_pd, VkQueueFlagBits vkqfbits);

/**
* After selecting a physical device to use.
* Set up a logical device to interface with your physical device
* This function is also used to set Vulkan Device Level Extensions
* that entail what a device does
*/
VkResult dlu_create_logical_device(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_pd,
  VkDeviceCreateFlags flags,
  uint32_t queueCreateInfoCount,
  const VkDeviceQueueCreateInfo *pQueueCreateInfos,
  VkPhysicalDeviceFeatures *pEnabledFeatures,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
);

/* Will retrieve a queue handle */
VkBool32 dlu_create_device_queue(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t queueIndex,
  VkQueueFlagBits vkqfbits
);

/**
* Create the actual swap chain used to present images to a surface.
* Does not check if image count exceeds the max
*/
VkResult dlu_create_swap_chain(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_scd,
  VkSwapchainCreateInfoKHR *create_info
);

/**
* Create image views which is the way you communicate to vulkan
* on how you intend to use and access VkImages
* It creates VkImageView handles for any particular image
* cur_index: Corresponds to the index of either vkcomp members text_data/sc_data
*/
VkResult dlu_create_image_views(dlu_image_view_type type, vkcomp *app, uint32_t cur_index, VkImageViewCreateInfo *img_view_info);

/**
* Need to depth buffer to render 3D images (only need one)
* Function will create the VkImage handle for the depth buffer
* Allocate memory and bind that memory to the VkImage Handle
* Then create the corresponding image view
*/
VkResult dlu_create_depth_buff(
  vkcomp *app,
  uint32_t cur_scd,
  VkImageCreateInfo *img_info,
  VkFlags requirements_mask
);

/**
* Function creates buffers like a uniform buffer so that shaders can access
* in a read-only fashion constant parameter data. Function also
* creates buffers like a vertex buffer so that it's visible to the CPU
*/
VkResult dlu_create_vk_buffer(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_bd,
  VkDeviceSize size,
  VkBufferCreateFlagBits flags,
  VkBufferUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  VkFlags requirements_mask
);

/**
* Attachments specified when creating the render pass
* are bounded by wrapping them into a VkFramebuffer object.
* A framebuffer object references all VkImageView objects this
* is represented by "attachments"
*/
VkResult dlu_create_framebuffers(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  VkImageView *pAttachments,
  uint32_t width,
  uint32_t height,
  uint32_t layers
);

/**
* Allows for your app to create a command pool to store your
* command buffers before being committed to main memory
*/
VkResult dlu_create_cmd_pool(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_scd,
  uint32_t cur_cmdd,
  uint32_t queueFamilyIndex,
  VkCommandPoolCreateFlagBits flags
);

/**
* Allows for your app to put commands into a buffer to later
* be submitted to one of the hardware queues
*/
VkResult dlu_create_cmd_buffs(
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
VkResult dlu_create_syncs(vkcomp *app, uint32_t cur_scd);

#endif
