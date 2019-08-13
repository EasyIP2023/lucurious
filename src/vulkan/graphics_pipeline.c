#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/vlucur/gp.h>
#include <wlu/utils/log.h>

VkShaderModule wlu_create_shader_module(vkcomp *app, const char *code, size_t code_size) {
  VkResult err;
  VkShaderModule shader_module = VK_NULL_HANDLE;

  if (!app->device) {
    wlu_log_me(WLU_DANGER, "[x] app->device must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return shader_module;
  }

  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code_size;
  create_info.pCode = (const uint32_t *) code;

  err = vkCreateShaderModule(app->device, &create_info, NULL, &shader_module);

  switch (err) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_OUT_OF_HOST_MEMORY");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_OUT_OF_DEVICE_MEMORY");
      break;
    case VK_ERROR_INVALID_SHADER_NV:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_INVALID_SHADER_NV");
      break;
    default:
      wlu_log_me(WLU_SUCCESS, "Shader module Successfully created");
      break;
  }

  return shader_module;
}

void wlu_freeup_shader(vkcomp *app, VkShaderModule shader_module) {
  vkDestroyShaderModule(app->device, shader_module, NULL);
  shader_module = NULL;
}

VkResult wlu_create_render_pass(
  vkcomp *app,
  uint32_t  attachmentCount,
  const VkAttachmentDescription *pAttachments,
  uint32_t subpassCount,
  const VkSubpassDescription *pSubpasses,
  uint32_t dependencyCount,
  const VkSubpassDependency *pDependencies
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->device) {
    wlu_log_me(WLU_DANGER, "[x] app->device must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.pNext = NULL;
  render_pass_info.flags = 0;
  render_pass_info.attachmentCount = attachmentCount;
  render_pass_info.pAttachments = pAttachments;
  render_pass_info.subpassCount = subpassCount;
  render_pass_info.pSubpasses = pSubpasses;
  render_pass_info.dependencyCount = dependencyCount;
  render_pass_info.pDependencies = pDependencies;

  res = vkCreateRenderPass(app->device, &render_pass_info, NULL, &app->render_pass);

  return res;
}

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
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->pipeline_layout || !app->render_pass) {
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_render_pass(3)");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_pipeline_layout(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.pNext = NULL;
  pipeline_info.stageCount = stageCount;
  pipeline_info.pStages = pStages;
  pipeline_info.pVertexInputState = pVertexInputState;
  pipeline_info.pInputAssemblyState = pInputAssemblyState;
  pipeline_info.pTessellationState = pTessellationState;
  pipeline_info.pViewportState = pViewportState;
  pipeline_info.pRasterizationState = pRasterizationState;
  pipeline_info.pMultisampleState = pMultisampleState;
  pipeline_info.pDepthStencilState = pDepthStencilState;
  pipeline_info.pColorBlendState = pColorBlendState;
  pipeline_info.pDynamicState = pDynamicState;
  pipeline_info.layout = app->pipeline_layout;
  pipeline_info.renderPass = app->render_pass;
  pipeline_info.subpass = subpass;
  pipeline_info.basePipelineHandle = basePipelineHandle;
  pipeline_info.basePipelineIndex = basePipelineIndex;

  res = vkCreateGraphicsPipelines(app->device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &app->graphics_pipeline);

  return res;
}

VkResult wlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
) {

  VkResult res;

  VkPipelineLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.setLayoutCount = app->desc_count;
  create_info.pSetLayouts = app->desc_layout;
  create_info.pushConstantRangeCount = pushConstantRangeCount;
  create_info.pPushConstantRanges = pPushConstantRanges;

  res = vkCreatePipelineLayout(app->device, &create_info, NULL, &app->pipeline_layout);

  return res;
}

void wlu_exec_begin_render_pass(
  vkcomp *app,
  uint32_t x,
  uint32_t y,
  VkExtent2D extent,
  uint32_t clear_value_count,
  const VkClearValue *pClearValues,
  VkSubpassContents contents
) {

  for (uint32_t i = 0; i < app->sc_buff_size; i++) {
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.pNext = NULL;
    render_pass_info.renderPass = app->render_pass;
    render_pass_info.framebuffer = app->sc_frame_buffs[i];
    render_pass_info.renderArea.offset.x = x;
    render_pass_info.renderArea.offset.y = y;
    render_pass_info.renderArea.extent = extent;
    render_pass_info.clearValueCount = clear_value_count;
    render_pass_info.pClearValues = pClearValues;

    vkCmdBeginRenderPass(app->cmd_buffs[i], &render_pass_info, contents);
  }
}

void wlu_exec_stop_render_pass(vkcomp *app) {
  for (uint32_t i = 0; i < app->sc_buff_size; i++)
    vkCmdEndRenderPass(app->cmd_buffs[i]);
}

void wlu_bind_gp(vkcomp *app, VkPipelineBindPoint pipelineBindPoint) {
  for (uint32_t i = 0; i < app->sc_buff_size; i++)
    vkCmdBindPipeline(app->cmd_buffs[i], pipelineBindPoint, app->graphics_pipeline);
}

void wlu_draw(
  vkcomp *app,
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance
) {
  for (uint32_t i = 0; i < app->sc_buff_size; i++)
    vkCmdDraw(app->cmd_buffs[i], vertexCount, instanceCount, firstVertex, firstInstance);
}

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

VkAttachmentReference wlu_set_attachment_ref(uint32_t attachment, VkImageLayout layout) {
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
  // subpass.flags = flags;
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
VkPipelineInputAssemblyStateCreateInfo wlu_set_input_assembly_state_info(VkPrimitiveTopology topology, VkBool32 pre) {
  VkPipelineInputAssemblyStateCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.topology = topology;
  create_info.primitiveRestartEnable = pre;

  return create_info;
}

/*
 * Describes the format of the vertex data passed to the vertex shader
 * two ways:
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
  VkViewport *viewport,
  uint32_t viewportCount,
  VkRect2D *scissor,
  uint32_t scissorCount
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

VkRect2D wlu_set_rect2D(uint32_t x, uint32_t y, VkExtent2D extent) {
  VkRect2D scissor = {};
  scissor.offset.x = x;
  scissor.offset.y = y;
  scissor.extent = extent;

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
  const VkSampler* pImmutableSamplers
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
  vkcomp *app,
  VkDescriptorSetLayoutCreateFlags flags,
  uint32_t bindingCount,
  const VkDescriptorSetLayoutBinding* pBindings
) {

  VkDescriptorSetLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.bindingCount = app->desc_count = bindingCount;
  create_info.pBindings = pBindings;

  return create_info;
}

VkResult wlu_create_desc_set_layout(
  vkcomp *app,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  app->desc_layout = (VkDescriptorSetLayout *) calloc(sizeof(VkDescriptorSetLayout),
        app->desc_count * sizeof(VkDescriptorSetLayout));
  if (!app->desc_layout) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkDescriptorSetLayout *desc_layout failed");
    return res;
  }

  res = vkCreateDescriptorSetLayout(app->device, desc_set_info, NULL, app->desc_layout);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateDescriptorSetLayout failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}

VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t psize,
  VkDescriptorPoolCreateFlags flags,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  VkDescriptorType descriptorType
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->desc_layout) {
    wlu_log_me(WLU_DANGER, "[x] Descriptor Set Layout not defined");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_desc_set_layout(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
  }

  VkDescriptorPoolSize pool_sizes[psize];
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = app->desc_count;

  VkDescriptorPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.maxSets = app->desc_count;
  create_info.poolSizeCount = psize;
  create_info.pPoolSizes = pool_sizes;

  res = vkCreateDescriptorPool(app->device, &create_info, NULL, &app->desc_pool);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateDescriptorPool failed, ERROR CODE: %d", res);
    return res;
  }

  VkDescriptorSetAllocateInfo alloc_info[psize];
  alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info[0].pNext = NULL;
  alloc_info[0].descriptorPool = app->desc_pool;
  alloc_info[0].descriptorSetCount = app->desc_count;
  alloc_info[0].pSetLayouts = app->desc_layout;

  app->desc_set = (VkDescriptorSet *) calloc(sizeof(VkDescriptorSet),
        app->desc_count * sizeof(VkDescriptorSet));
  if (!app->desc_set) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkDescriptorSet *desc_set failed");
    return res = VK_RESULT_MAX_ENUM;
  }

  res = vkAllocateDescriptorSets(app->device, alloc_info, app->desc_set);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateDescriptorSets failed, ERROR CODE: %d", res);
    return res;
  }

  VkWriteDescriptorSet writes[psize];
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].pNext = NULL;
  writes[0].dstSet = app->desc_set[0];
  writes[0].dstBinding = dstBinding;
  writes[0].dstArrayElement = dstArrayElement;
  writes[0].descriptorCount = app->desc_count;
  writes[0].descriptorType = descriptorType;
  writes[0].pImageInfo = NULL;
  writes[0].pBufferInfo = &app->uniform_data.buff_info;
  writes[0].pImageInfo = NULL;

  vkUpdateDescriptorSets(app->device, psize, writes, 0, NULL);

  return res;
}
