/*
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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/vlucur/gp.h>
#include <wlu/utils/log.h>

VkAttachmentDescription wlu_set_attachment_desc(
  VkFormat format,
  VkSampleCountFlagBits samples,
  VkAttachmentLoadOp loadOp,
  VkAttachmentStoreOp storeOp,
  VkAttachmentLoadOp stencilLoadOp,
  VkAttachmentStoreOp stencilStoreOp,
  VkImageLayout initialLayout,
  VkImageLayout finalLayout
) {

  VkAttachmentDescription color_attachment = {};
  color_attachment.format = format;
  color_attachment.samples = samples;
  color_attachment.loadOp = loadOp;
  color_attachment.storeOp = storeOp;
  color_attachment.stencilLoadOp = stencilLoadOp;
  color_attachment.stencilStoreOp = stencilStoreOp;
  color_attachment.initialLayout = initialLayout;
  color_attachment.finalLayout = finalLayout;

  return color_attachment;
}

VkAttachmentReference wlu_set_attachment_ref(
  uint32_t attachment, VkImageLayout layout
) {

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = attachment;
  color_attachment_ref.layout = layout;

  return color_attachment_ref;
}

VkSubpassDescription wlu_set_subpass_desc(
  uint32_t inputAttachmentCount,
  const VkAttachmentReference *pInputAttachments,
  uint32_t colorAttachmentCount,
  const VkAttachmentReference *pColorAttachments,
  const VkAttachmentReference *pResolveAttachments,
  const VkAttachmentReference *pDepthStencilAttachment,
  uint32_t preserveAttachmentCount,
  const uint32_t *pPreserveAttachments
) {

  VkSubpassDescription subpass = {};
  subpass.flags = 0;
  /* used to indicate if this is a graphics or a compute subpass */
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.inputAttachmentCount = inputAttachmentCount;
  subpass.pInputAttachments = pInputAttachments;
  subpass.colorAttachmentCount = colorAttachmentCount;
  subpass.pColorAttachments = pColorAttachments;
  subpass.pResolveAttachments = pResolveAttachments;
  subpass.pDepthStencilAttachment = pDepthStencilAttachment;
  subpass.preserveAttachmentCount = preserveAttachmentCount;
  subpass.pPreserveAttachments = pPreserveAttachments;

  return subpass;
}

VkSubpassDependency wlu_set_subpass_dep(
  uint32_t srcSubpass,
  uint32_t dstSubpass,
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkAccessFlags srcAccessMask,
  VkAccessFlags dstAccessMask,
  VkDependencyFlags dependencyFlags
){

  VkSubpassDependency dep = {};
  dep.srcSubpass = srcSubpass;
  dep.dstSubpass = dstSubpass;
  dep.srcStageMask = srcStageMask;
  dep.dstStageMask = dstStageMask;
  dep.srcAccessMask = srcAccessMask;
  dep.dstAccessMask = dstAccessMask;
  dep.dependencyFlags = dependencyFlags;

  return dep;
}

/* Allows for actual use of the shaders we created */
VkPipelineShaderStageCreateInfo wlu_set_shader_stage_info(
  VkShaderModule mod,
  const char *pName,
  VkShaderStageFlagBits stage,
  const VkSpecializationInfo *pSpecializationInfo
) {

  VkPipelineShaderStageCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.stage = stage;
  create_info.module = mod;
  create_info.pName = pName;
  create_info.pSpecializationInfo = pSpecializationInfo;

  return create_info;
}

/*
 * Defines what kind of geometry will be drawn from the vertices and
 * if primitive restart should be enable
 */
VkPipelineInputAssemblyStateCreateInfo wlu_set_input_assembly_state_info(
  VkPrimitiveTopology topology, VkBool32 pre
) {
  VkPipelineInputAssemblyStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.topology = topology;
  create_info.primitiveRestartEnable = pre;

  return create_info;
}

VkVertexInputBindingDescription wlu_set_vertex_input_binding_desc(
  uint32_t binding, uint32_t stride, VkVertexInputRate inputRate
) {

  VkVertexInputBindingDescription binding_desc = {};
  binding_desc.binding = binding;
  binding_desc.stride = stride;
  binding_desc.inputRate = inputRate;

  return binding_desc;
}

VkVertexInputAttributeDescription wlu_set_vertex_input_attrib_desc(
  uint32_t location, uint32_t binding, VkFormat format, uint32_t offset
) {

  VkVertexInputAttributeDescription attrib_desc = {};
  attrib_desc.location = location;
  attrib_desc.binding = binding;
  attrib_desc.format = format;
  attrib_desc.offset = offset;

  return attrib_desc;
}

/*
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
) {

  VkPipelineVertexInputStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
  create_info.pVertexBindingDescriptions = pVertexBindingDescriptions;
  create_info.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
  create_info.pVertexAttributeDescriptions = pVertexAttributeDescriptions;

  return create_info;
}

/* Decribe the region of the framebuffer that the output is rendered to */
VkViewport wlu_set_view_port(float x, float y, float width, float height, float minDepth, float maxDepth) {

  VkViewport viewport = {};
  viewport.x = x;
  viewport.y = y;
  viewport.width = width;
  viewport.height = height;
  viewport.minDepth = minDepth;
  viewport.maxDepth = maxDepth;

  return viewport;
}

VkPipelineViewportStateCreateInfo wlu_set_view_port_state_info(
  uint32_t viewportCount,
  VkViewport *viewport,
  uint32_t scissorCount,
  VkRect2D *scissor
) {

  VkPipelineViewportStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.viewportCount = viewportCount;
  create_info.pViewports = viewport;
  create_info.scissorCount = scissorCount;
  create_info.pScissors = scissor;

  return create_info;
}

VkRect2D wlu_set_rect2D(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  VkRect2D scissor = {};
  scissor.offset.x = x;
  scissor.offset.y = y;
  scissor.extent.width = width;
  scissor.extent.height = height;

  return scissor;
}

/*
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
) {

  VkPipelineRasterizationStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  /*
   * If set to VK_TRUE, then fragments that are beyond the near and far
   * planes are clamped to them as opposed to discarding them.
   */
  create_info.depthClampEnable = depthClampEnable;
  /*
   * If set to VK_TRUE, then geometry never passes through the rasterizer stage.
   * This basically disables any output to the framebuffer.
   */
  create_info.rasterizerDiscardEnable = rasterizerDiscardEnable;
  create_info.polygonMode = polygonMode; /* determines how fragments are generated for geometry */
  create_info.cullMode = cullMode;
  create_info.frontFace = frontFace;
  create_info.depthBiasEnable = depthBiasEnable;
  create_info.depthBiasConstantFactor = depthBiasConstantFactor;
  create_info.depthBiasClamp = depthBiasClamp;
  create_info.depthBiasSlopeFactor = depthBiasSlopeFactor;

  create_info.lineWidth = lineWidth; /* Describes the thickness of the line segments */

  return create_info;
}

/* One of the best possible ways to perform anti-aliasing */
VkPipelineMultisampleStateCreateInfo wlu_set_multisample_state_info(
  VkSampleCountFlagBits rasterizationSamples,
  VkBool32 sampleShadingEnable,
  float minSampleShading,
  const VkSampleMask *pSampleMask,
  VkBool32 alphaToCoverageEnable,
  VkBool32 alphaToOneEnable
) {

  VkPipelineMultisampleStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.rasterizationSamples = rasterizationSamples;
  create_info.sampleShadingEnable = sampleShadingEnable;
  create_info.minSampleShading = minSampleShading;
  create_info.pSampleMask = pSampleMask;
  create_info.alphaToCoverageEnable = alphaToCoverageEnable;
  create_info.alphaToOneEnable = alphaToOneEnable;

  return create_info;
}

VkStencilOpState wlu_set_stencil_op_state(
  VkStencilOp failOp,
  VkStencilOp passOp,
  VkStencilOp depthFailOp,
  VkCompareOp compareOp,
  uint32_t compareMask,
  uint32_t writeMask,
  uint32_t reference
) {

  VkStencilOpState stencil_state = {};
  stencil_state.failOp = failOp;
  stencil_state.passOp = passOp;
  stencil_state.depthFailOp = depthFailOp;
  stencil_state.compareOp = compareOp;
  stencil_state.compareMask = compareMask;
  stencil_state.writeMask = writeMask;
  stencil_state.reference = reference;

  return stencil_state;
}

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
) {

  VkPipelineDepthStencilStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.depthTestEnable = depthTestEnable;
  create_info.depthWriteEnable = depthWriteEnable;
  create_info.depthCompareOp = depthCompareOp;
  create_info.depthBoundsTestEnable = depthBoundsTestEnable;
  create_info.stencilTestEnable = stencilTestEnable;
  create_info.front = front;
  create_info.back = back;
  create_info.minDepthBounds = minDepthBounds;
  create_info.maxDepthBounds = maxDepthBounds;

  return create_info;
}


/*
 * Once a fragment shader has returned a color,
 * colorblending combines the color that is already in the framebuffer
 * Done by:
 * Mixing the old and new values to produce a final color
 * Combining the old and new values using a bitwise operation
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
) {

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.blendEnable = blendEnable;
  color_blend_attachment.srcColorBlendFactor = srcColorBlendFactor;
  color_blend_attachment.dstColorBlendFactor = dstColorBlendFactor;
  color_blend_attachment.colorBlendOp = colorBlendOp;
  color_blend_attachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
  color_blend_attachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
  color_blend_attachment.alphaBlendOp = alphaBlendOp;
  color_blend_attachment.colorWriteMask = colorWriteMask;

  return color_blend_attachment;
}

VkPipelineColorBlendStateCreateInfo wlu_set_color_blend_attachment_state_info(
  VkBool32 logicOpEnable,
  VkLogicOp logicOp,
  uint32_t attachmentCount,
  const VkPipelineColorBlendAttachmentState *pAttachments,
  float blendConstants[4]
) {

  VkPipelineColorBlendStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.logicOpEnable = logicOpEnable;
  create_info.logicOp = logicOp;
  create_info.attachmentCount = attachmentCount;
  create_info.pAttachments = pAttachments;
  create_info.blendConstants[0] = blendConstants[0];
  create_info.blendConstants[1] = blendConstants[1];
  create_info.blendConstants[2] = blendConstants[2];
  create_info.blendConstants[3] = blendConstants[3];

  return create_info;
}

VkPipelineDynamicStateCreateInfo wlu_set_dynamic_state_info(
  uint32_t dynamicStateCount,
  const VkDynamicState *pDynamicStates
) {

  VkPipelineDynamicStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.dynamicStateCount = dynamicStateCount;
  create_info.pDynamicStates = pDynamicStates;

  return create_info;
}

VkDescriptorSetLayoutBinding wlu_set_desc_set(
  uint32_t binding,
  VkDescriptorType descriptorType,
  uint32_t descriptorCount,
  VkShaderStageFlags stageFlags,
  const VkSampler *pImmutableSamplers
) {

  VkDescriptorSetLayoutBinding create_info = {};
  create_info.binding = binding;
  create_info.descriptorType = descriptorType;
  create_info.descriptorCount = descriptorCount;
  create_info.stageFlags = stageFlags;
  create_info.pImmutableSamplers = pImmutableSamplers;

  return create_info;
}

VkDescriptorSetLayoutCreateInfo wlu_set_desc_set_info(
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding *pBindings
) {

  VkDescriptorSetLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.bindingCount = bindingCount;
  create_info.pBindings = pBindings;

  return create_info;
}

VkClearValue wlu_set_clear_value(
  float float32[4],
  int32_t int32[4],
  uint32_t uint32[4],
  float depth,
  uint32_t stencil
) {

  VkClearValue clear_value;
  clear_value.color.float32[0] = float32[0];
  clear_value.color.float32[1] = float32[1];
  clear_value.color.float32[2] = float32[2];
  clear_value.color.float32[3] = float32[3];
  clear_value.color.int32[0] = int32[0];
  clear_value.color.int32[1] = int32[1];
  clear_value.color.int32[2] = int32[2];
  clear_value.color.int32[3] = int32[3];
  clear_value.color.uint32[0] = uint32[0];
  clear_value.color.uint32[1] = uint32[1];
  clear_value.color.uint32[2] = uint32[2];
  clear_value.color.uint32[3] = uint32[3];
  clear_value.depthStencil.depth   = depth;
  clear_value.depthStencil.stencil = stencil;

  return clear_value;
}
