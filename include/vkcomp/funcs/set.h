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

#ifndef WLU_VKCOMP_SET_FUNCS_H
#define WLU_VKCOMP_SET_FUNCS_H

/**
* Usage of functions in set.h files is optional
* Added set functions as an extra options for those
* who don't want to fill out the entire VK struct themselves
*/

VkCommandBufferInheritanceInfo wlu_set_cmd_buff_inheritance_info(
  VkRenderPass renderPass,
  uint32_t subpass,
  VkFramebuffer framebuffer,
  VkBool32 occlusionQueryEnable,
  VkQueryControlFlags queryFlags,
  VkQueryPipelineStatisticFlags pipelineStatistics
);

VkImageSubresourceRange wlu_set_image_sub_resource_range(
  VkImageAspectFlags aspectMask,
  uint32_t baseMipLevel,
  uint32_t levelCount,
  uint32_t baseArrayLayer,
  uint32_t layerCount
);

/* set info for resource synchronization access */
VkImageMemoryBarrier wlu_set_image_mem_barrier(
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkImageLayout oldLayout,
  VkImageLayout newLayout,
  uint32_t srcQueueFamilyIndex,
  uint32_t dstQueueFamilyIndex,
  VkImage image,
  VkImageSubresourceRange subresourceRange
);

VkImageCreateInfo wlu_set_image_info(
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
);

VkImageViewCreateInfo wlu_set_image_view_info(
  VkImageViewCreateFlags flags,
  VkImage image,
  VkImageViewType viewType,
  VkFormat format,
  VkComponentMapping components,
  VkImageSubresourceRange subresourceRange
);

VkComponentMapping wlu_set_component_mapping(
  VkComponentSwizzle r,
  VkComponentSwizzle g,
  VkComponentSwizzle b,
  VkComponentSwizzle a
);

/* Allows for multiple different copies of from one buffer to the image in one operation */
VkBufferImageCopy wlu_set_buff_image_copy(
  VkDeviceSize bufferOffset,
  uint32_t bufferRowLength,
  uint32_t bufferImageHeight,
  VkImageSubresourceLayers imageSubresource,
  VkOffset3D imageOffset,
  VkExtent3D imageExtent
);

VkImageSubresourceLayers wlu_set_image_sub_resource_layers(
  VkImageAspectFlags aspectMask,
  uint32_t mipLevel,
  uint32_t baseArrayLayer,
  uint32_t layerCount
);

VkExtent2D wlu_set_extent2D(uint32_t width, uint32_t height);

VkExtent3D wlu_set_extent3D(uint32_t width, uint32_t height, uint32_t depth);

VkOffset3D wlu_set_offset3D(int32_t x, int32_t y, int32_t z);

VkSamplerCreateInfo wlu_set_sampler(
  VkSamplerCreateFlags flags,
  VkFilter magFilter,
  VkFilter minFilter,
  float mipLodBias,
  VkSamplerMipmapMode mipmapMode,
  VkSamplerAddressMode addressModeU,
  VkSamplerAddressMode addressModeV,
  VkSamplerAddressMode addressModeW,
  float maxAnisotropy,
  VkBool32 anisotropyEnable,
  VkBool32 compareEnable,
  VkCompareOp compareOp,
  float minLod,
  float maxLod,
  VkBorderColor borderColor,
  VkBool32 unnormalizedCoordinates
);

#endif
