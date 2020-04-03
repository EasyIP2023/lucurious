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

VkShaderModule wlu_create_shader_module(vkcomp *app, char *code, size_t code_size) {
  VkResult err;
  VkShaderModule shader_module = VK_NULL_HANDLE;

  if (!app->device) { PERR(WLU_VKCOMP_DEVICE, 0, NULL); return shader_module; }

  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.codeSize = code_size;
  create_info.pCode = (const uint32_t *) code;

  err = vkCreateShaderModule(app->device, &create_info, NULL, &shader_module);
  if (err) { PERR(WLU_VK_FUNC_ERR, err, "vkCreateShaderModule"); }

  if (err == VK_SUCCESS)
    wlu_log_me(WLU_SUCCESS, "Shader module successfully created");

  return shader_module;
}

VkResult wlu_create_render_pass(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  const VkAttachmentDescription *pAttachments,
  uint32_t subpassCount,
  const VkSubpassDescription *pSubpasses,
  uint32_t dependencyCount,
  const VkSubpassDependency *pDependencies
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->device) { PERR(WLU_VKCOMP_DEVICE, 0, NULL); return res; }
  if (!app->gp_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_GP_DATA"); return res; }

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

  res = vkCreateRenderPass(app->device, &render_pass_info, NULL, &app->gp_data[cur_gpd].render_pass);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateRenderPass"); }

  return res;
}

VkResult wlu_create_graphics_pipelines(
  vkcomp *app,
  uint32_t cur_gpd,
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

  if (!app->gp_data[cur_gpd].render_pass) { PERR(WLU_VKCOMP_RENDER_PASS, 0, NULL); return res; }
  if (!app->gp_data[cur_gpd].pipeline_layout) { PERR(WLU_VKCOMP_PIPELINE_LAYOUT, 0, NULL); return res; }
  if (!app->gp_data[cur_gpd].graphics_pipelines) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_GP_DATA_MEMS"); return res; }

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
  pipeline_info.layout = app->gp_data[cur_gpd].pipeline_layout;
  pipeline_info.renderPass = app->gp_data[cur_gpd].render_pass;
  pipeline_info.subpass = subpass;
  pipeline_info.basePipelineHandle = basePipelineHandle;
  pipeline_info.basePipelineIndex = basePipelineIndex;

  res = vkCreateGraphicsPipelines(app->device, app->pipeline_cache, 1, &pipeline_info, NULL, app->gp_data[cur_gpd].graphics_pipelines);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateGraphicsPipelines"); }

  return res;
}

VkResult wlu_create_pipeline_cache(vkcomp *app, size_t initialDataSize, const void *pInitialData) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkPipelineCacheCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  create_info.flags = 0;
  create_info.pNext = NULL;
  create_info.initialDataSize = initialDataSize;
  create_info.pInitialData = pInitialData;

  res = vkCreatePipelineCache(app->device, &create_info, NULL, &app->pipeline_cache);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreatePipelineCache"); }

  return res;
}

VkResult wlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t cur_dd,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->gp_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_GP_DATA"); return res; }

  VkPipelineLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.pushConstantRangeCount = pushConstantRangeCount;
  create_info.pPushConstantRanges = pPushConstantRanges;

  /* Function may be called multiple times and descriptor data struct array members may not be needed */
  if (app->desc_data) {
    create_info.setLayoutCount = app->desc_data[cur_dd].dlsc;
    create_info.pSetLayouts = app->desc_data[cur_dd].layouts;
  } else {
    create_info.setLayoutCount = 0;
    create_info.pSetLayouts = NULL;
  }

  res = vkCreatePipelineLayout(app->device, &create_info, NULL, &app->gp_data[cur_gpd].pipeline_layout);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkCreatePipelineLayout")

  return res;
}

VkResult wlu_create_desc_set_layouts(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->desc_data[cur_dd].layouts) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_DESC_DATA_MEMS"); return res; }

  for (uint32_t i = 0; i < app->desc_data[cur_dd].dlsc; i++) {
    res = vkCreateDescriptorSetLayout(app->device, desc_set_info, NULL, &app->desc_data[cur_dd].layouts[i]);
    if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateDescriptorSetLayout"); return res; }
  }

  return res;
}

VkResult wlu_create_desc_pool(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorPoolCreateFlags flags,
  uint32_t psize,
  VkDescriptorPoolSize *pool_sizes
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkDescriptorPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.maxSets = app->desc_data[cur_dd].dlsc;
  create_info.poolSizeCount = psize;
  create_info.pPoolSizes = pool_sizes;

  res = vkCreateDescriptorPool(app->device, &create_info, NULL, &app->desc_data[cur_dd].desc_pool);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkCreateDescriptorPool")

  return res;
}

VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t cur_dd
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->desc_data[cur_dd].desc_pool) { PERR(WLU_VKCOMP_DESC_POOL, 0, NULL); return res; }
  /* Leaving this error check here for now */
  if (!app->desc_data[cur_dd].layouts) { PERR(WLU_VKCOMP_DESC_LAYOUT, 0, NULL); return res; }
  if (!app->desc_data[cur_dd].desc_set) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_DESC_DATA_MEMS"); return res; }

  VkDescriptorSetAllocateInfo alloc_info;
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.descriptorPool = app->desc_data[cur_dd].desc_pool;
  alloc_info.descriptorSetCount = app->desc_data[cur_dd].dlsc;
  alloc_info.pSetLayouts = app->desc_data[cur_dd].layouts;

  res = vkAllocateDescriptorSets(app->device, &alloc_info, app->desc_data[cur_dd].desc_set);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkAllocateDescriptorSets")

  return res;
}
