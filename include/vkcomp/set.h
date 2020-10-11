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

#ifndef DLU_VKCOMP_SET_H
#define DLU_VKCOMP_SET_H

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

static inline VkComponentMapping dlu_set_component_mapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a) {

  return (VkComponentMapping) { .r = r, .g = g, .b = b, .a = a };
}

static inline VkAttachmentDescription dlu_set_attachment_desc(
  VkFormat format,
  VkSampleCountFlagBits samples,
  VkAttachmentLoadOp loadOp,
  VkAttachmentStoreOp storeOp,
  VkAttachmentLoadOp stencilLoadOp,
  VkAttachmentStoreOp stencilStoreOp,
  VkImageLayout initialLayout,
  VkImageLayout finalLayout
) {

  return (VkAttachmentDescription) {
         .format = format, .samples = samples, .loadOp = loadOp, .storeOp = storeOp, .stencilLoadOp = stencilLoadOp,
         .stencilStoreOp = stencilStoreOp, .initialLayout = initialLayout, .finalLayout = finalLayout
  };
}

static inline VkAttachmentReference dlu_set_attachment_ref(uint32_t attachment, VkImageLayout layout) {
  return (VkAttachmentReference) { .attachment = attachment, .layout = layout };
}

static inline VkSubpassDescription dlu_set_subpass_desc(
  VkSubpassDescriptionFlags flags,
  VkPipelineBindPoint pipelineBindPoint,
  uint32_t inputAttachmentCount,
  const VkAttachmentReference *pInputAttachments,
  uint32_t colorAttachmentCount,
  const VkAttachmentReference *pColorAttachments,
  const VkAttachmentReference *pResolveAttachments,
  const VkAttachmentReference *pDepthStencilAttachment,
  uint32_t preserveAttachmentCount,
  const uint32_t *pPreserveAttachments
) {

  return (VkSubpassDescription) {
         .flags = flags, .pipelineBindPoint = pipelineBindPoint, .inputAttachmentCount = inputAttachmentCount, .pInputAttachments = pInputAttachments,
         .colorAttachmentCount = colorAttachmentCount, .pColorAttachments = pColorAttachments, .pResolveAttachments = pResolveAttachments,
         .pDepthStencilAttachment = pDepthStencilAttachment, .preserveAttachmentCount = preserveAttachmentCount, .pPreserveAttachments = pPreserveAttachments
  };
}

static inline VkSubpassDependency dlu_set_subpass_dep(
  uint32_t srcSubpass,
  uint32_t dstSubpass,
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkDependencyFlags dependencyFlags
) {

  return (VkSubpassDependency) {
         .srcSubpass = srcSubpass, .dstSubpass = dstSubpass, .srcStageMask = srcStageMask, .dstStageMask = dstStageMask,
         .srcAccessMask = srcAccessMask, .dstAccessMask = dstAccessMask, .dependencyFlags = dependencyFlags
  };
}

/* Allows for actual use of the shaders we created */
static inline VkPipelineShaderStageCreateInfo dlu_set_shader_stage_info(
  VkShaderModule module,
  const char *pName,
  VkShaderStageFlagBits stage,
  const VkSpecializationInfo *pSpecializationInfo,
  VkPipelineShaderStageCreateFlags flags
) {

  return (VkPipelineShaderStageCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
         .pNext = NULL, .flags = flags, .stage = stage, .module = module,
         .pName = pName, .pSpecializationInfo = pSpecializationInfo
  };
}

/**
* Defines what kind of geometry will be drawn from the vertices and
* if primitive restart should be enable
*/
static inline VkPipelineInputAssemblyStateCreateInfo dlu_set_input_assembly_state_info(
  VkPipelineInputAssemblyStateCreateFlags flags,
  VkPrimitiveTopology topology,
  VkBool32 primitiveRestartEnable
) {

  return (VkPipelineInputAssemblyStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
         .pNext = NULL, .flags = flags, .topology = topology, .primitiveRestartEnable = primitiveRestartEnable
  };
}

/**
* binding: Specifiy the shader vertex attribute variable binding
* stride: Specify the size in bytes of the shader vertex attribute variables
* inputRate: Describe at which rate to load data from memory throughout the vertices
*/
static inline VkVertexInputBindingDescription dlu_set_vertex_input_binding_desc(
  uint32_t binding, uint32_t stride, VkVertexInputRate inputRate
) {

  return (VkVertexInputBindingDescription) { .binding = binding, .stride = stride, .inputRate = inputRate };
}

static inline VkVertexInputAttributeDescription dlu_set_vertex_input_attrib_desc(
  uint32_t location, uint32_t binding, VkFormat format, uint32_t offset
) {

  return (VkVertexInputAttributeDescription) { .location = location, .binding = binding, .format = format, .offset = offset };
}

/**
* Describes the format of the vertex data passed to the vertex shader
* Two Ways:
* Bindings: spacing between data and whether the data is per-vertex or per-instance (see instancing)
* Attribute descriptions: type of the attributes passed to the vertex shader,
* which binding to load them from and at which offset
*/
static inline VkPipelineVertexInputStateCreateInfo dlu_set_vertex_input_state_info(
  uint32_t vertexBindingDescriptionCount,
  const VkVertexInputBindingDescription *pVertexBindingDescriptions,
  uint32_t vertexAttributeDescriptionCount,
  const VkVertexInputAttributeDescription *pVertexAttributeDescriptions
) {

  return (VkPipelineVertexInputStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .vertexBindingDescriptionCount = vertexBindingDescriptionCount, .pVertexBindingDescriptions = pVertexBindingDescriptions,
         .vertexAttributeDescriptionCount = vertexAttributeDescriptionCount, .pVertexAttributeDescriptions = pVertexAttributeDescriptions
  };
}

/* specify which region of a framebuffer should be rendered to an output */
static inline VkViewport dlu_set_view_port(float x, float y, float width, float height, float minDepth, float maxDepth) {

  return (VkViewport) { .x = x, .y = y, .width = width, .height = height, .minDepth = minDepth, .maxDepth = maxDepth };
}

static inline VkPipelineViewportStateCreateInfo dlu_set_view_port_state_info(
  uint32_t viewportCount,
  VkViewport *viewport,
  uint32_t scissorCount,
  VkRect2D *scissor
) {

  return (VkPipelineViewportStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .viewportCount = viewportCount, .pViewports = viewport,
         .scissorCount = scissorCount, .pScissors = scissor
  };
}

static inline VkRect2D dlu_set_rect2D(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  return (VkRect2D) { .offset.x = x, .offset.y = y, .extent.width = width, .extent.height = height };
}

/**
* Rasterizer takes the geometry shaped by the vertex/geometry/tesellation shaders
* and turns it into fragments to be colored by the fragment shader.
* It also performs depth testing, face culling and the scissor test.
* Can be configured to output fragments that fill entire polygons
* depthClampEnable: If set to VK_TRUE, then fragments that are beyond the near and far
*                   planes are clamped to them as opposed to discarding them.
* rasterizerDiscardEnable: If set to VK_TRUE, then geometry never passes through the rasterizer stage.
*                          This basically disables any output to the framebuffer.
* polygonMode: Determines how fragments are generated for geometry
* lineWidth: Describes the thickness of the line segments
*/
static inline VkPipelineRasterizationStateCreateInfo dlu_set_rasterization_state_info(
  VkBool32 depthClampEnable,
  VkBool32 rasterizerDiscardEnable,
  VkPolygonMode polygonMode,
  VkCullModeFlags cullMode,
  VkFrontFace frontFace,
  VkBool32 depthBiasEnable,
  float depthBiasConstantFactor,
  float depthBiasClamp,
  float depthBiasSlopeFactor,
  float lineWidth
) {

  return (VkPipelineRasterizationStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .depthClampEnable = depthClampEnable, .rasterizerDiscardEnable = rasterizerDiscardEnable, .polygonMode = polygonMode,
         .cullMode = cullMode, .frontFace = frontFace, .depthBiasEnable = depthBiasEnable, .depthBiasConstantFactor = depthBiasConstantFactor,
         .depthBiasClamp = depthBiasClamp, .depthBiasSlopeFactor = depthBiasSlopeFactor, .lineWidth = lineWidth
  };
}

/* Used for anti-aliasing */
static inline VkPipelineMultisampleStateCreateInfo dlu_set_multisample_state_info(
  VkSampleCountFlagBits rasterizationSamples,
  VkBool32 sampleShadingEnable,
  float minSampleShading,
  const VkSampleMask *pSampleMask,
  VkBool32 alphaToCoverageEnable,
  VkBool32 alphaToOneEnable
) {

  return (VkPipelineMultisampleStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .rasterizationSamples = rasterizationSamples, .sampleShadingEnable = sampleShadingEnable,
         .minSampleShading = minSampleShading, .pSampleMask = pSampleMask, .alphaToCoverageEnable = alphaToCoverageEnable,
         .alphaToOneEnable = alphaToOneEnable
  };
}

static inline VkStencilOpState dlu_set_stencil_op_state(
  VkStencilOp failOp,
  VkStencilOp passOp,
  VkStencilOp depthFailOp,
  VkCompareOp compareOp,
  uint32_t compareMask,
  uint32_t writeMask,
  uint32_t reference
) {

  return (VkStencilOpState) {
         .failOp = failOp, .passOp = passOp, .depthFailOp = depthFailOp, .compareOp = compareOp,
         .compareMask = compareMask, .writeMask = writeMask, .reference = reference
  };
}

static inline VkPipelineDepthStencilStateCreateInfo dlu_set_depth_stencil_state(
  VkBool32 depthTestEnable,
  VkBool32 depthWriteEnable,
  VkCompareOp depthCompareOp,
  VkBool32 depthBoundsTestEnable,
  VkBool32 stencilTestEnable,
  VkStencilOpState front,
  VkStencilOpState back,
  float minDepthBounds,
  float maxDepthBounds
) {

  return (VkPipelineDepthStencilStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .depthTestEnable = depthTestEnable, .depthWriteEnable = depthWriteEnable, .depthCompareOp = depthCompareOp,
         .depthBoundsTestEnable = depthBoundsTestEnable, .stencilTestEnable = stencilTestEnable, .front = front, .back = back, .minDepthBounds = minDepthBounds, .maxDepthBounds = maxDepthBounds
  };
}

/**
* Once a fragment shader has returned a color,
* colorblending combines the color that is already in the framebuffer
* Done by:
* Mixing the old and new values to produce a final color
* Combining the old and new values using a bitwise operation
*/
static inline VkPipelineColorBlendAttachmentState dlu_set_color_blend_attachment_state(
  VkBool32 blendEnable,
  VkBlendFactor srcColorBlendFactor,
  VkBlendFactor dstColorBlendFactor,
  VkBlendOp colorBlendOp,
  VkBlendFactor srcAlphaBlendFactor,
  VkBlendFactor dstAlphaBlendFactor,
  VkBlendOp alphaBlendOp,
  VkColorComponentFlags colorWriteMask
) {

  return (VkPipelineColorBlendAttachmentState) {
         .blendEnable = blendEnable, .srcColorBlendFactor = srcColorBlendFactor, .dstColorBlendFactor = dstColorBlendFactor, .colorBlendOp = colorBlendOp,
         .srcAlphaBlendFactor = srcAlphaBlendFactor, .dstAlphaBlendFactor = dstAlphaBlendFactor, .alphaBlendOp = alphaBlendOp, .colorWriteMask = colorWriteMask
  };
}

static inline VkPipelineColorBlendStateCreateInfo dlu_set_color_blend_attachment_state_info(
  VkBool32 logicOpEnable,
  VkLogicOp logicOp,
  uint32_t attachmentCount,
  const VkPipelineColorBlendAttachmentState *pAttachments,
  float blendConstants[4]
) {

  return (VkPipelineColorBlendStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .logicOpEnable = logicOpEnable, .logicOp = logicOp, .attachmentCount = attachmentCount, .pAttachments = pAttachments,
         .blendConstants[0] = blendConstants[0], .blendConstants[1] = blendConstants[1], .blendConstants[2] = blendConstants[2], .blendConstants[3] = blendConstants[3]
  };
}

static inline VkPipelineDynamicStateCreateInfo dlu_set_dynamic_state_info(
  uint32_t dynamicStateCount,
  const VkDynamicState *pDynamicStates
) {

  return (VkPipelineDynamicStateCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
         .pNext = NULL, .flags = 0, .dynamicStateCount = dynamicStateCount, .pDynamicStates = pDynamicStates
  };
}

static inline VkDescriptorSetLayoutBinding dlu_set_desc_set_layout_binding(
  uint32_t binding,
  VkDescriptorType descriptorType,
  uint32_t descriptorCount,
  VkShaderStageFlags stageFlags,
  const VkSampler *pImmutableSamplers
) {

  return (VkDescriptorSetLayoutBinding) {
         .binding = binding, .descriptorType = descriptorType, .descriptorCount = descriptorCount,
         .stageFlags = stageFlags, .pImmutableSamplers = pImmutableSamplers
  };
}

static inline VkDescriptorSetLayoutCreateInfo dlu_set_desc_set_layout_info(
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding *pBindings
) {

  return (VkDescriptorSetLayoutCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
         .pNext = NULL, .flags = flags, .bindingCount = bindingCount, .pBindings = pBindings
  };
}

static inline VkDescriptorPoolSize dlu_set_desc_pool_size(VkDescriptorType type, uint32_t descriptorCount) {

  return (VkDescriptorPoolSize) { .type = type, .descriptorCount = descriptorCount };
}

static inline VkDescriptorBufferInfo dlu_set_desc_buff_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {

  return (VkDescriptorBufferInfo) { .buffer = buffer, .offset = offset, .range = range };
}

static inline VkClearValue dlu_set_clear_value(
  float float32[4],
  int32_t int32[4],
  uint32_t uint32[4],
  float depth,
  uint32_t stencil
) {

  return (VkClearValue) {
         .color.float32[0] = float32[0], .color.float32[1] = float32[1], .color.float32[2] = float32[2], .color.float32[3] = float32[3],
         .color.int32[0] = int32[0], .color.int32[1] = int32[1], .color.int32[2] = int32[2], .color.int32[3] = int32[3],
         .color.uint32[0] = uint32[0], .color.uint32[1] = uint32[1], .color.uint32[2] = uint32[2], .color.uint32[3] = uint32[3],
        .depthStencil.depth = depth, .depthStencil.stencil = stencil
  };
}

static inline VkDescriptorImageInfo dlu_set_desc_img_info(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout) {

  return (VkDescriptorImageInfo) { .sampler = sampler, .imageView = imageView, .imageLayout = imageLayout };
}

static inline VkSamplerCreateInfo dlu_set_sampler_info(
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
) {

  return (VkSamplerCreateInfo) {
         .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
         .pNext = NULL, .flags = flags, .magFilter = magFilter, .minFilter = minFilter, .mipmapMode = mipmapMode,
         .addressModeU = addressModeU, .addressModeV = addressModeV, .addressModeW = addressModeW,
         .mipLodBias = mipLodBias, .anisotropyEnable = anisotropyEnable, .maxAnisotropy = maxAnisotropy,
         .compareEnable = compareEnable, .compareOp = compareOp, .minLod = minLod, .maxLod = maxLod,
         .borderColor = borderColor, .unnormalizedCoordinates = unnormalizedCoordinates
  };
}

/**
* descriptorCount: Specify the amount of descriptors to update
* Tell the driver that which resource to use for descriptors in a set
*/
static inline VkWriteDescriptorSet dlu_set_write_desc_set(
  VkDescriptorSet dstSet,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  uint32_t descriptorCount,
  VkDescriptorType descriptorType,
  const VkDescriptorImageInfo *pImageInfo,
  const VkDescriptorBufferInfo *pBufferInfo,
  const VkBufferView *pTexelBufferView
) {

  return (VkWriteDescriptorSet) { 
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = NULL, .dstSet = dstSet, .dstBinding = dstBinding, .dstArrayElement = dstArrayElement,
    .descriptorCount = descriptorCount, .descriptorType = descriptorType, .pImageInfo = pImageInfo,
    .pBufferInfo = pBufferInfo, .pTexelBufferView = pTexelBufferView
  };
}

#endif
