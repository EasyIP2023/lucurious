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

#ifndef WLU_VKCOMP_GP_SET_H
#define WLU_VKCOMP_GP_SET_H

/**
* Usage of functions in set.h files is optional
* Added set functions as an extra options for those
* who don't want to fill out the entire VK struct themselves
*/

VkAttachmentDescription wlu_set_attachment_desc(
  VkFormat format,
  VkSampleCountFlagBits samples,
  VkAttachmentLoadOp loadOp,
  VkAttachmentStoreOp storeOp,
  VkAttachmentLoadOp stencilLoadOp,
  VkAttachmentStoreOp stencilStoreOp,
  VkImageLayout initialLayout,
  VkImageLayout finalLayout
);

VkAttachmentReference wlu_set_attachment_ref(
  uint32_t attachment, VkImageLayout layout
);

VkSubpassDescription wlu_set_subpass_desc(
  VkPipelineBindPoint pipelineBindPoint,
  uint32_t inputAttachmentCount,
  const VkAttachmentReference *pInputAttachments,
  uint32_t colorAttachmentCount,
  const VkAttachmentReference *pColorAttachments,
  const VkAttachmentReference *pResolveAttachments,
  const VkAttachmentReference *pDepthStencilAttachment,
  uint32_t preserveAttachmentCount,
  const uint32_t *pPreserveAttachments
);

VkSubpassDependency wlu_set_subpass_dep(
  uint32_t srcSubpass,
  uint32_t dstSubpass,
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkDependencyFlags dependencyFlags
);

VkPipelineShaderStageCreateInfo wlu_set_shader_stage_info(
  VkShaderModule mod,
  const char *pName,
  VkShaderStageFlagBits stage,
  const VkSpecializationInfo *pSpecializationInfo
);

VkPipelineInputAssemblyStateCreateInfo wlu_set_input_assembly_state_info(
  VkPrimitiveTopology topology, VkBool32 pre
);

/* Describe at which rate to load data from memory throughout the vertices */
VkVertexInputBindingDescription wlu_set_vertex_input_binding_desc(
  uint32_t binding, uint32_t stride, VkVertexInputRate inputRate
);

VkVertexInputAttributeDescription wlu_set_vertex_input_attrib_desc(
  uint32_t location, uint32_t binding, VkFormat format, uint32_t offset
);

/**
* Describes the format of the vertex data passed to the vertex shader
* Two Ways:
* Bindings: spacing between data and whether the data is per-vertex or per-instance (see instancing)
* Attribute descriptions: type of the attributes passed to the vertex shader,
* which binding to load them from and at which offset
*/
VkPipelineVertexInputStateCreateInfo wlu_set_vertex_input_state_info(
  uint32_t vertexBindingDescriptionCount,
  const VkVertexInputBindingDescription *pVertexBindingDescriptions,
  uint32_t vertexAttributeDescriptionCount,
  const VkVertexInputAttributeDescription *pVertexAttributeDescriptions
);

/* specify which region of a framebuffer to an output should render to */
VkViewport wlu_set_view_port(
  float x, float y, float width, float height,
  float minDepth, float maxDepth
);

VkPipelineViewportStateCreateInfo wlu_set_view_port_state_info(
  uint32_t viewportCount,
  VkViewport *viewport,
  uint32_t scissorCount,
  VkRect2D *scissor
);

VkRect2D wlu_set_rect2D(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/**
* Rasterizer takes the geometry shaped by the vertices from the vertex shader
* and turns it into fragments to be colored by the fragment shader.
* It also performs depth testing, face culling and the scissor test.
* Can be configured to output fragments that fill entire polygons
*/
VkPipelineRasterizationStateCreateInfo wlu_set_rasterization_state_info(
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
);

/* Used for anti-aliasing */
VkPipelineMultisampleStateCreateInfo wlu_set_multisample_state_info(
  VkSampleCountFlagBits rasterizationSamples,
  VkBool32 sampleShadingEnable,
  float minSampleShading,
  const VkSampleMask *pSampleMask,
  VkBool32 alphaToCoverageEnable,
  VkBool32 alphaToOneEnable
);

VkStencilOpState wlu_set_stencil_op_state(
  VkStencilOp failOp,
  VkStencilOp passOp,
  VkStencilOp depthFailOp,
  VkCompareOp compareOp,
  uint32_t compareMask,
  uint32_t writeMask,
  uint32_t reference
);

VkPipelineDepthStencilStateCreateInfo wlu_set_depth_stencil_state(
  VkBool32 depthTestEnable,
  VkBool32 depthWriteEnable,
  VkCompareOp depthCompareOp,
  VkBool32 depthBoundsTestEnable,
  VkBool32 stencilTestEnable,
  VkStencilOpState front,
  VkStencilOpState back,
  float minDepthBounds,
  float maxDepthBounds
);

/**
* After fragment shader returns color one needs to combine it
* with a color that is already in the framebuffer
*/
VkPipelineColorBlendAttachmentState wlu_set_color_blend_attachment_state(
  VkBool32 blendEnable,
  VkBlendFactor srcColorBlendFactor,
  VkBlendFactor dstColorBlendFactor,
  VkBlendOp colorBlendOp,
  VkBlendFactor srcAlphaBlendFactor,
  VkBlendFactor dstAlphaBlendFactor,
  VkBlendOp alphaBlendOp,
  VkColorComponentFlags colorWriteMask
);

VkPipelineColorBlendStateCreateInfo wlu_set_color_blend_attachment_state_info(
  VkBool32 logicOpEnable,
  VkLogicOp logicOp,
  uint32_t attachmentCount,
  const VkPipelineColorBlendAttachmentState *pAttachments,
  float blendConstants[4]
);

VkPipelineDynamicStateCreateInfo wlu_set_dynamic_state_info(
  uint32_t dynamicStateCount,
  const VkDynamicState *pDynamicStates
);

VkDescriptorSetLayoutBinding wlu_set_desc_set_layout_binding(
  uint32_t binding,
  VkDescriptorType descriptorType,
  uint32_t descriptorCount,
  VkShaderStageFlags stageFlags,
  const VkSampler *pImmutableSamplers
);

VkDescriptorSetLayoutCreateInfo wlu_set_desc_set_layout_info(
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding *pBindings
);

VkDescriptorPoolSize wlu_set_desc_pool_size(
  VkDescriptorType type,
  uint32_t descriptorCount
);

VkDescriptorBufferInfo wlu_set_desc_buff_info(
  VkBuffer buffer,
  VkDeviceSize offset,
  VkDeviceSize range
);

VkClearValue wlu_set_clear_value(
  float float32[4],
  int32_t int32[4],
  uint32_t uint32[4],
  float depth,
  uint32_t stencil
);

VkDescriptorImageInfo wlu_set_desc_img_info(
  VkSampler sampler,
  VkImageView imageView,
  VkImageLayout imageLayout
);

#endif
