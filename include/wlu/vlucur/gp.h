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

#ifndef WLU_GRAPHICS_PIPELINE_H
#define WLU_GRAPHICS_PIPELINE_H

VkShaderModule wlu_create_shader_module(vkcomp *app, const char *code, size_t code_size);

void wlu_freeup_shader(vkcomp *app, VkShaderModule shader_module);

VkResult wlu_create_render_pass(
  vkcomp *app,
  uint32_t  attachmentCount,
  const VkAttachmentDescription *pAttachments,
  uint32_t subpassCount,
  const VkSubpassDescription *pSubpasses,
  uint32_t dependencyCount,
  const VkSubpassDependency *pDependencies
);

VkResult wlu_create_graphics_pipeline(
  vkcomp *app,
  uint32_t stageCount,
  const VkPipelineShaderStageCreateInfo *pStages,
  const VkPipelineVertexInputStateCreateInfo *pVertexInputState,
  const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState,
  const VkPipelineTessellationStateCreateInfo *pTessellationState,
  const VkPipelineViewportStateCreateInfo *pViewportState,
  const VkPipelineRasterizationStateCreateInfo *pRasterizationState,
  const VkPipelineMultisampleStateCreateInfo *pMultisampleState,
  const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState,
  const VkPipelineColorBlendStateCreateInfo *pColorBlendState,
  const VkPipelineDynamicStateCreateInfo *pDynamicState,
  uint32_t subpass,
  VkPipeline basePipelineHandle,
  uint32_t basePipelineIndex
);

VkResult wlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
);

void wlu_exec_begin_render_pass(
  vkcomp *app,
  uint32_t x,
  uint32_t y,
  VkExtent2D extent,
  uint32_t clear_value_count,
  const VkClearValue *pClearValues,
  VkSubpassContents contents
);

void wlu_exec_stop_render_pass(vkcomp *app);

void wlu_bind_gp(vkcomp *app, VkPipelineBindPoint pipelineBindPoint);

void wlu_draw(
  vkcomp *app,
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance
);

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

VkPipelineVertexInputStateCreateInfo wlu_set_vertex_input_state_info(
  uint32_t vertexBindingDescriptionCount,
  const VkVertexInputBindingDescription *pVertexBindingDescriptions,
  uint32_t vertexAttributeDescriptionCount,
  const VkVertexInputAttributeDescription *pVertexAttributeDescriptions
);

VkViewport wlu_set_view_port(
  float x, float y, float width,
  float height, float minDepth,
  float maxDepth
);

VkPipelineViewportStateCreateInfo wlu_set_view_port_state_info(
  VkViewport *viewport,
  uint32_t viewportCount,
  VkRect2D *scissor,
  uint32_t scissorCount
);

VkRect2D wlu_set_rect2D(uint32_t x, uint32_t y, VkExtent2D extent);

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

VkPipelineMultisampleStateCreateInfo wlu_set_multisample_state_info(
  VkSampleCountFlagBits rasterizationSamples,
  VkBool32 sampleShadingEnable,
  float minSampleShading,
  const VkSampleMask *pSampleMask,
  VkBool32 alphaToCoverageEnable,
  VkBool32 alphaToOneEnable
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

VkDescriptorSetLayoutBinding wlu_set_desc_set(
  uint32_t binding,
  VkDescriptorType descriptorType,
  uint32_t descriptorCount,
  VkShaderStageFlags stageFlags,
  const VkSampler* pImmutableSamplers
);

VkDescriptorSetLayoutCreateInfo wlu_set_desc_set_info(
  vkcomp *app,
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding* pBindings
);

VkResult wlu_create_desc_set_layout(
  vkcomp *app,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
);

VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t psize,
  VkDescriptorPoolCreateFlags flags,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  VkDescriptorType descriptorType
);

#endif
