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

void wlu_freeup_shader(vkcomp *app, VkShaderModule *shader_module);

VkShaderModule wlu_create_shader_module(vkcomp *app, char *code, size_t code_size);

VkResult wlu_create_render_pass(
  vkcomp *app,
  uint32_t attachmentCount,
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

VkResult wlu_create_pipeline_cache(vkcomp *app, size_t initialDataSize, const void *pInitialData);

VkResult wlu_create_pipeline_layout(vkcomp *app, uint32_t pushConstantRangeCount, const VkPushConstantRange *pPushConstantRanges);

VkResult wlu_create_desc_set_layout(
  vkcomp *app,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
);

/*
 * Function creates a descriptor set that you can use to
 * inform the GPU how the data contained in a buffer (Uniform)
 * is mapped to the shader program's variables
 */
VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t psize,
  uint32_t maxSets,
  VkDescriptorPoolCreateFlags flags,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  VkDescriptorType descriptorType,
  VkDescriptorBufferInfo *pBufferInfo
);

void wlu_exec_begin_render_pass(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height,
  uint32_t clearValueCount,
  const VkClearValue *pClearValues,
  VkSubpassContents contents
);

void wlu_exec_stop_render_pass(vkcomp *app, uint32_t cur_pool);

void wlu_bind_pipeline(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  VkPipelineBindPoint pipelineBindPoint,
  VkPipeline pipeline
);

void wlu_bind_desc_set(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  VkPipelineBindPoint pipelineBindPoint,
  uint32_t firstSet,
  uint32_t dynamicOffsetCount,
  const uint32_t *pDynamicOffsets
);

void wlu_bind_vertex_buff_to_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t firstBinding,
  uint32_t bindingCount,
  const VkBuffer *pBuffers,
  const VkDeviceSize *offsets
);

void wlu_cmd_draw(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance
);

void wlu_cmd_draw_indexed(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t indexCount,
  uint32_t instanceCount,
  uint32_t firstIndex,
  int32_t vertexOffset,
  uint32_t firstInstance
);

void wlu_cmd_set_viewport(
  vkcomp *app,
  VkViewport viewport,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t firstViewport,
  uint32_t viewportCount
);

void wlu_cmd_set_scissor(
  vkcomp *app,
  VkRect2D scissor,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t firstScissor,
  uint32_t scissorCount
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

/* Describe at which rate to load data from memory throughout the vertices */
VkVertexInputBindingDescription wlu_set_vertex_input_binding_desc(
  uint32_t binding, uint32_t stride, VkVertexInputRate inputRate
);

VkVertexInputAttributeDescription wlu_set_vertex_input_attrib_desc(
  uint32_t location, uint32_t binding, VkFormat format, uint32_t offset
);

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

/*
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

VkDescriptorSetLayoutBinding wlu_set_desc_set(
  vkcomp *app,
  uint32_t binding,
  VkDescriptorType descriptorType,
  uint32_t descriptorCount,
  VkShaderStageFlags stageFlags,
  const VkSampler *pImmutableSamplers
);

VkDescriptorSetLayoutCreateInfo wlu_set_desc_set_info(
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding *pBindings
);

VkClearValue wlu_set_clear_value(
  float float32[4],
  int32_t int32[4],
  uint32_t uint32[4],
  float depth,
  uint32_t stencil
);

#endif
