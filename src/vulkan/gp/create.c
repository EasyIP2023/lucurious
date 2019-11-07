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
#include <vlucur/values.h>
#include <wlu/utils/log.h>

void wlu_freeup_shader(vkcomp *app, VkShaderModule *shader_module) {
  if (*shader_module) {
    vkDestroyShaderModule(app->device, *shader_module, NULL);
    *shader_module = VK_NULL_HANDLE;
  }
}

VkShaderModule wlu_create_shader_module(vkcomp *app, char *code, size_t code_size) {
  VkResult err;
  VkShaderModule shader_module = VK_NULL_HANDLE;

  if (!app->device) {
    wlu_log_me(WLU_DANGER, "[x] A logical device must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device()");
    return shader_module;
  }

  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.codeSize = code_size;
  create_info.pCode = (const uint32_t *) code;

  err = vkCreateShaderModule(app->device, &create_info, NULL, &shader_module);

  switch (err) {
    case VK_SUCCESS:
      wlu_log_me(WLU_SUCCESS, "Shader module successfully created");
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_OUT_OF_HOST_MEMORY");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_OUT_OF_DEVICE_MEMORY");
      break;
    case VK_ERROR_INVALID_SHADER_NV:
      wlu_log_me(WLU_DANGER, "[x] failed to create shader module! VK_ERROR_INVALID_SHADER_NV");
      break;
    default: break;
  }

  FREE(code);
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

  if (!app->device) {
    wlu_log_me(WLU_DANGER, "[x] A Vulkan Logical Device must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device()");
    return res;
  }

  if (!app->gp_data) {
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_gp_data()");
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

  res = vkCreateRenderPass(app->device, &render_pass_info, NULL, &app->gp_data[cur_gpd].render_pass);

  return res;
}

VkResult wlu_create_gp_data(vkcomp *app) {
  app->gp_data = realloc(app->gp_data, (app->gpc+1) * sizeof(struct graphics_pipeline_data));
  if (!app->gp_data) {
    wlu_log_me(WLU_DANGER, "realloc app->gp_data failed!!");
    return VK_RESULT_MAX_ENUM;
  }

  set_gp_data_init_values(app);
  app->gpc++;

  return VK_SUCCESS;
}

VkResult wlu_create_graphics_pipelines(
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
  uint32_t basePipelineIndex,
  uint32_t cur_gpd,
  uint32_t gps_count /* graphics pipelines count */
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->gp_data[cur_gpd].render_pass) {
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_render_pass()");
    return res;
  }

  if (!app->gp_data[cur_gpd].pipeline_layout) {
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_pipeline_layout()");
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
  pipeline_info.layout = app->gp_data[cur_gpd].pipeline_layout;
  pipeline_info.renderPass = app->gp_data[cur_gpd].render_pass;
  pipeline_info.subpass = subpass;
  pipeline_info.basePipelineHandle = basePipelineHandle;
  pipeline_info.basePipelineIndex = basePipelineIndex;

  app->gp_data[cur_gpd].graphics_pipelines = (VkPipeline *) calloc(sizeof(VkPipeline), gps_count * sizeof(VkPipeline));
  if (!app->gp_data[cur_gpd].graphics_pipelines) {
    wlu_log_me(WLU_DANGER, "calloc app->gp_data[%d].graphics_pipelines falied", cur_gpd);
    return res;
  }

  res = vkCreateGraphicsPipelines(app->device, app->pipeline_cache, 1, &pipeline_info, NULL, app->gp_data[cur_gpd].graphics_pipelines);

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

  return res;
}

VkResult wlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t setLayoutCount,
  VkDescriptorSetLayout *pSetLayouts,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->gp_data) {
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_gp_data()");
    return res;
  }

  VkPipelineLayoutCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.setLayoutCount = setLayoutCount;
  create_info.pSetLayouts = pSetLayouts;
  create_info.pushConstantRangeCount = pushConstantRangeCount;
  create_info.pPushConstantRanges = pPushConstantRanges;

  res = vkCreatePipelineLayout(app->device, &create_info, NULL, &app->gp_data[cur_gpd].pipeline_layout);

  return res;
}

VkResult wlu_create_desc_data(vkcomp *app, uint32_t desc_count) {

  app->desc_data = (struct descriptors *) realloc(app->desc_data,
      (app->ddc+1) * sizeof(struct descriptors));
  if (!app->desc_data) {
    wlu_log_me(WLU_DANGER, "realloc app->desc_data failed!");
    return VK_RESULT_MAX_ENUM;
  }

  set_desc_data_init_values(app);
  app->desc_data[app->ddc].dc = desc_count;
  app->ddc++;

  return VK_SUCCESS;
}

VkResult wlu_create_desc_set_layout(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorSetLayoutCreateInfo *desc_set_info /* Using same layout for all obects for now */
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  app->desc_data[cur_dd].desc_layouts = calloc(sizeof(VkDescriptorSetLayout),
        app->desc_data[cur_dd].dc * sizeof(VkDescriptorSetLayout));
  if (!app->desc_data[cur_dd].desc_layouts) {
    wlu_log_me(WLU_DANGER, "calloc app->desc_data[cur_dd].desc_layouts failed", cur_dd);
    return res;
  }

  for (uint32_t i = 0; i < app->desc_data[cur_dd].dc; i++) {
    res = vkCreateDescriptorSetLayout(app->device, desc_set_info, NULL,
                                      &app->desc_data[cur_dd].desc_layouts[i]);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateDescriptorSetLayout failed, ERROR CODE: %d", res);
      goto finish_desc_layout;
    }
  }

finish_desc_layout:
  return res;
}

VkResult wlu_create_desc_pool(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorPoolCreateFlags flags,
  uint32_t psize
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkDescriptorPoolSize pool_sizes[psize];
  for (uint32_t i = 0; i < psize; i++) {
    pool_sizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[i].descriptorCount = app->desc_data[cur_dd].dc;
  }

  VkDescriptorPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.maxSets = app->desc_data[cur_dd].dc;
  create_info.poolSizeCount = psize;
  create_info.pPoolSizes = pool_sizes;

  res = vkCreateDescriptorPool(app->device, &create_info, NULL, &app->desc_data[cur_dd].desc_pool);

  return res;
}

VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t cur_dd,
  uint32_t psize
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->desc_data[cur_dd].desc_pool) {
    wlu_log_me(WLU_DANGER, "[x] In order to allocate descriptor sets one must have a descriptor pool");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_desc_pool()");
    return res;
  }

  VkDescriptorSetAllocateInfo alloc_info[psize];
  for (uint32_t i = 0; i < psize; i++) {
    alloc_info[i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[i].pNext = NULL;
    alloc_info[i].descriptorPool = app->desc_data[cur_dd].desc_pool;
    alloc_info[i].descriptorSetCount = psize;
    alloc_info[i].pSetLayouts = app->desc_data[cur_dd].desc_layouts; // For now
  }

  app->desc_data[cur_dd].desc_set = (VkDescriptorSet *) calloc(sizeof(VkDescriptorSet),
            app->desc_data[cur_dd].dc * sizeof(VkDescriptorSet));
  if (!app->desc_data[cur_dd].desc_set) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkDescriptorSet *desc_set failed");
    return VK_RESULT_MAX_ENUM;
  }

  res = vkAllocateDescriptorSets(app->device, alloc_info, app->desc_data[cur_dd].desc_set);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateDescriptorSets failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}
