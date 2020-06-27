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

#define LUCUR_VKCOMP_API
#include <lucom.h>

VkDeviceQueueCreateInfo dlu_set_device_queue_info(
  VkDeviceQueueCreateFlags flags,
  uint32_t queueFamilyIndex,
  uint32_t queueCount,
  const float *pQueuePriorities
) {
  
  VkDeviceQueueCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueFamilyIndex = queueFamilyIndex;
  create_info.queueCount = queueCount;
  create_info.pQueuePriorities = pQueuePriorities;

  return create_info;
}

VkCommandBufferInheritanceInfo dlu_set_cmd_buff_inheritance_info(
  VkRenderPass renderPass,
  uint32_t subpass,
  VkFramebuffer framebuffer,
  VkBool32 occlusionQueryEnable,
  VkQueryControlFlags queryFlags,
  VkQueryPipelineStatisticFlags pipelineStatistics
) {

  VkCommandBufferInheritanceInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  create_info.pNext = NULL;
  create_info.renderPass = renderPass;
  create_info.subpass = subpass;
  create_info.framebuffer = framebuffer;
  create_info.occlusionQueryEnable = occlusionQueryEnable;
  create_info.queryFlags = queryFlags;
  create_info.pipelineStatistics = pipelineStatistics;

  return create_info;
}

VkSwapchainCreateInfoKHR dlu_set_swap_chain_info(
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

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.pNext = pNext;
  create_info.flags = flags;
  create_info.surface = surface;
  create_info.minImageCount = minImageCount;
  create_info.imageFormat = imageFormat;
  create_info.imageColorSpace = imageColorSpace;
  create_info.imageExtent = imageExtent;
  create_info.imageArrayLayers = imageArrayLayers;
  create_info.imageUsage = imageUsage;
  create_info.imageSharingMode = imageSharingMode;
  create_info.queueFamilyIndexCount = queueFamilyIndexCount;
  create_info.pQueueFamilyIndices = pQueueFamilyIndices;
  create_info.preTransform = preTransform;
  create_info.compositeAlpha = compositeAlpha;
  create_info.presentMode = presentMode;
  create_info.clipped = clipped;
  create_info.oldSwapchain = oldSwapchain;

  return create_info;
}

VkImageSubresourceRange dlu_set_image_sub_resource_range(
  VkImageAspectFlags aspectMask,
  uint32_t baseMipLevel,
  uint32_t levelCount,
  uint32_t baseArrayLayer,
  uint32_t layerCount
) {

  VkImageSubresourceRange create_info = {};
  create_info.aspectMask = aspectMask;
  create_info.baseMipLevel = baseMipLevel;
  create_info.levelCount = levelCount;
  create_info.baseArrayLayer = baseArrayLayer;
  create_info.layerCount = layerCount;

  return create_info;
}

VkImageMemoryBarrier dlu_set_image_mem_barrier(
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkImageLayout oldLayout,
  VkImageLayout newLayout,
  uint32_t srcQueueFamilyIndex,
  uint32_t dstQueueFamilyIndex,
  VkImage image,
  VkImageSubresourceRange subresourceRange
) {

  VkImageMemoryBarrier create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  create_info.pNext = NULL;
  create_info.srcAccessMask = srcAccessMask;
  create_info.dstAccessMask = dstAccessMask;
  create_info.oldLayout = oldLayout;
  create_info.newLayout = newLayout;
  create_info.srcQueueFamilyIndex = srcQueueFamilyIndex;
  create_info.dstQueueFamilyIndex = dstQueueFamilyIndex;
  create_info.image = image;
  create_info.subresourceRange = subresourceRange;

  return create_info;
}

VkImageCreateInfo dlu_set_image_info(
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

  VkImageCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.imageType = imageType;
  create_info.format = format;
  create_info.extent.width = extent.width;
  create_info.extent.height = extent.height;
  create_info.extent.depth = extent.depth;
  create_info.mipLevels = mipLevels;
  create_info.arrayLayers = arrayLayers;
  create_info.samples = samples;
  create_info.tiling = tiling;
  create_info.usage = usage;
  create_info.sharingMode = sharingMode;
  create_info.queueFamilyIndexCount = queueFamilyIndexCount;
  create_info.pQueueFamilyIndices = pQueueFamilyIndices;
  create_info.initialLayout = initialLayout;

  return create_info;
}

VkImageViewCreateInfo dlu_set_image_view_info(
  VkImageViewCreateFlags flags,
  VkImage image,
  VkImageViewType viewType,
  VkFormat format,
  VkComponentMapping components,
  VkImageSubresourceRange subresourceRange
) {

  VkImageViewCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.image = image;
  create_info.viewType = viewType;
  create_info.format = format;
  create_info.components = components;
  create_info.subresourceRange = subresourceRange;

  return create_info;
}

VkComponentMapping dlu_set_component_mapping(
  VkComponentSwizzle r,
  VkComponentSwizzle g,
  VkComponentSwizzle b,
  VkComponentSwizzle a
) {

  VkComponentMapping create_info = {.r = r, .g = g, .b = b, .a = a};

  return create_info;
}

VkBufferImageCopy dlu_set_buff_image_copy(
  VkDeviceSize bufferOffset,
  uint32_t bufferRowLength,
  uint32_t bufferImageHeight,
  VkImageSubresourceLayers imageSubresource,
  VkOffset3D imageOffset,
  VkExtent3D imageExtent
) {

  VkBufferImageCopy create_info = {};
  create_info.bufferOffset = bufferOffset;
  create_info.bufferRowLength = bufferRowLength;
  create_info.bufferImageHeight = bufferImageHeight;
  create_info.imageSubresource = imageSubresource;
  create_info.imageOffset.x = imageOffset.x;
  create_info.imageOffset.y = imageOffset.y;
  create_info.imageOffset.z = imageOffset.z;
  create_info.imageExtent.width = imageExtent.width;
  create_info.imageExtent.height = imageExtent.height;
  create_info.imageExtent.depth = imageExtent.depth;

  return create_info;
}

VkImageSubresourceLayers dlu_set_image_sub_resource_layers(
  VkImageAspectFlags aspectMask,
  uint32_t mipLevel,
  uint32_t baseArrayLayer,
  uint32_t layerCount
) {

  VkImageSubresourceLayers create_info = {};
  create_info.aspectMask = aspectMask;
  create_info.mipLevel = mipLevel;
  create_info.baseArrayLayer = baseArrayLayer;
  create_info.layerCount = layerCount;

  return create_info;
}

VkExtent2D dlu_set_extent2D(uint32_t width, uint32_t height) {

  VkExtent2D create_info = { .width = width, .height = height };

  return create_info;
}

VkExtent3D dlu_set_extent3D(uint32_t width, uint32_t height, uint32_t depth) {

  VkExtent3D create_info = { .width = width, .height = height, .depth = depth };

  return create_info;
}

VkOffset3D dlu_set_offset3D(int32_t x, int32_t y, int32_t z) {

  VkOffset3D create_info = { .x = x, .y = y, .z = z };

  return create_info;
}
