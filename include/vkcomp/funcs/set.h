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

#ifndef DLU_VKCOMP_SET_FUNCS_H
#define DLU_VKCOMP_SET_FUNCS_H

/**
* Usage of functions in set.h files is optional
* Added set functions as an extra options for those
* who don't want to fill out the entire VK struct themselves
*/

static inline VkDeviceQueueCreateInfo dlu_set_device_queue_info(
  VkDeviceQueueCreateFlags flags,
  uint32_t queueFamilyIndex,
  uint32_t queueCount,
  const float *pQueuePriorities
) {

  return (VkDeviceQueueCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .pNext = NULL, .flags = flags, .queueFamilyIndex = queueFamilyIndex,
         .queueCount = queueCount, .pQueuePriorities = pQueuePriorities
  };
}

static inline VkCommandBufferInheritanceInfo dlu_set_cmd_buff_inheritance_info(
  VkRenderPass renderPass,
  uint32_t subpass,
  VkFramebuffer framebuffer,
  VkBool32 occlusionQueryEnable,
  VkQueryControlFlags queryFlags,
  VkQueryPipelineStatisticFlags pipelineStatistics
) {

  return (VkCommandBufferInheritanceInfo) {
         .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
         .pNext = NULL, .renderPass = renderPass, .subpass = subpass, .framebuffer = framebuffer,
         .occlusionQueryEnable = occlusionQueryEnable, .queryFlags = queryFlags, .pipelineStatistics = pipelineStatistics
  };
}

static inline VkSwapchainCreateInfoKHR dlu_set_swap_chain_info(
  const void *pNext,
  VkSwapchainCreateFlagsKHR flags,
  VkSurfaceKHR surface,
  uint32_t minImageCount,
  VkFormat imageFormat,
  VkColorSpaceKHR imageColorSpace,
  VkExtent2D imageExtent,
  uint32_t imageArrayLayers,
  VkImageUsageFlags imageUsage,
  VkSharingMode imageSharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  VkSurfaceTransformFlagBitsKHR preTransform,
  VkCompositeAlphaFlagBitsKHR compositeAlpha,
  VkPresentModeKHR presentMode,
  VkBool32 clipped,
  VkSwapchainKHR oldSwapchain
) {

  return (VkSwapchainCreateInfoKHR) {
         .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
         .pNext = pNext, .flags = flags, .surface = surface, .minImageCount = minImageCount, .imageFormat = imageFormat, .imageColorSpace = imageColorSpace,
         .imageExtent = imageExtent, .imageArrayLayers = imageArrayLayers, .imageUsage = imageUsage, .imageSharingMode = imageSharingMode,
         .queueFamilyIndexCount = queueFamilyIndexCount, .pQueueFamilyIndices = pQueueFamilyIndices, .preTransform = preTransform,
         .compositeAlpha = compositeAlpha, .presentMode = presentMode, .clipped = clipped, .oldSwapchain = oldSwapchain
  };
}

static inline VkImageSubresourceRange dlu_set_image_sub_resource_range(
  VkImageAspectFlags aspectMask,
  uint32_t baseMipLevel,
  uint32_t levelCount,
  uint32_t baseArrayLayer,
  uint32_t layerCount
) {

  return (VkImageSubresourceRange) {
         .aspectMask = aspectMask, .baseMipLevel = baseMipLevel, .levelCount = levelCount,
         .baseArrayLayer = baseArrayLayer, .layerCount = layerCount
  };
}

/* set info for resource (i.e VkBuffer, VkImage) synchronization access */ 
static inline VkImageMemoryBarrier dlu_set_image_mem_barrier(
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkImageLayout oldLayout,
  VkImageLayout newLayout,
  uint32_t srcQueueFamilyIndex,
  uint32_t dstQueueFamilyIndex,
  VkImage image,
  VkImageSubresourceRange subresourceRange
) {

  return (VkImageMemoryBarrier) {
         .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
         .pNext = NULL, .srcAccessMask = srcAccessMask, .dstAccessMask = dstAccessMask, .oldLayout = oldLayout,
         .newLayout = newLayout, .srcQueueFamilyIndex = srcQueueFamilyIndex, .dstQueueFamilyIndex = dstQueueFamilyIndex,
         .image = image, .subresourceRange = subresourceRange
  };
}

static inline VkBufferMemoryBarrier dlu_set_buffer_mem_barrier(
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  uint32_t srcQueueFamilyIndex,
  uint32_t dstQueueFamilyIndex,
  VkBuffer buffer,
  VkDeviceSize offset,
  VkDeviceSize size
) {

  return (VkBufferMemoryBarrier) {
         .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
         .pNext = NULL, .srcAccessMask = srcAccessMask, .dstAccessMask = dstAccessMask,
         .srcQueueFamilyIndex = srcQueueFamilyIndex, .dstQueueFamilyIndex = dstQueueFamilyIndex,
         .buffer = buffer, .offset = offset, .size = size
  };

}

static inline VkImageCreateInfo dlu_set_image_info(
  VkImageCreateFlags flags,
  VkImageType imageType,
  VkFormat format,
  VkExtent3D extent,
  uint32_t mipLevels,
  uint32_t arrayLayers,
  VkSampleCountFlagBits samples,
  VkImageTiling tiling,
  VkImageUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  VkImageLayout initialLayout
) {

  return (VkImageCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
         .pNext = NULL, .flags = flags, .imageType = imageType, .format = format, .extent.width = extent.width, .extent.height = extent.height,
         .extent.depth = extent.depth, .mipLevels = mipLevels, .arrayLayers = arrayLayers, .samples = samples, .tiling = tiling, .usage = usage,
         .sharingMode = sharingMode, .queueFamilyIndexCount = queueFamilyIndexCount, .pQueueFamilyIndices = pQueueFamilyIndices,
         .initialLayout = initialLayout,
  };
}

static inline VkImageViewCreateInfo dlu_set_image_view_info(
  VkImageViewCreateFlags flags,
  VkImage image,
  VkImageViewType viewType,
  VkFormat format,
  VkComponentMapping components,
  VkImageSubresourceRange subresourceRange
) {

  return (VkImageViewCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
         .pNext = NULL, .flags = flags, .image = image, .viewType = viewType, .format = format,
         .components = components, .subresourceRange = subresourceRange
  };
}

static inline VkComponentMapping dlu_set_component_mapping(
  VkComponentSwizzle r,
  VkComponentSwizzle g,
  VkComponentSwizzle b,
  VkComponentSwizzle a
) {

  return (VkComponentMapping) {.r = r, .g = g, .b = b, .a = a};
}

/* Allows for multiple different copies of from one buffer to the image in one operation */
static inline VkBufferImageCopy dlu_set_buff_image_copy(
  VkDeviceSize bufferOffset,
  uint32_t bufferRowLength,
  uint32_t bufferImageHeight,
  VkImageSubresourceLayers imageSubresource,
  VkOffset3D imageOffset,
  VkExtent3D imageExtent
) {

  return (VkBufferImageCopy) {
         .bufferOffset = bufferOffset, .bufferRowLength = bufferRowLength, .bufferImageHeight = bufferImageHeight, .imageSubresource = imageSubresource,
         .imageOffset.x = imageOffset.x, .imageOffset.y = imageOffset.y, .imageOffset.z = imageOffset.z, .imageExtent.width = imageExtent.width,
         .imageExtent.height = imageExtent.height, .imageExtent.depth = imageExtent.depth
  };
}

static inline VkImageSubresourceLayers dlu_set_image_sub_resource_layers(
  VkImageAspectFlags aspectMask,
  uint32_t mipLevel,
  uint32_t baseArrayLayer,
  uint32_t layerCount
) {

  return (VkImageSubresourceLayers) { .aspectMask = aspectMask, .mipLevel = mipLevel, .baseArrayLayer = baseArrayLayer, .layerCount = layerCount };
}

static inline VkExtent2D dlu_set_extent2D(uint32_t width, uint32_t height) { return (VkExtent2D) { .width = width, .height = height }; }

static inline VkExtent3D dlu_set_extent3D(uint32_t width, uint32_t height, uint32_t depth) { return (VkExtent3D) { .width = width, .height = height, .depth = depth }; }

static inline VkOffset3D dlu_set_offset3D(int32_t x, int32_t y, int32_t z) { return (VkOffset3D) { .x = x, .y = y, .z = z }; }

#endif
