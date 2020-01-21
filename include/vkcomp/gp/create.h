/**
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

#ifndef WLU_GP_CREATE_H
#define WLU_GP_CREATE_H

void wlu_freeup_shader(vkcomp *app, VkShaderModule shader_module);

VkShaderModule wlu_create_shader_module(vkcomp *app, char *code, size_t code_size);

VkResult wlu_create_render_pass(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  const VkAttachmentDescription *pAttachments,
  uint32_t subpassCount,
  const VkSubpassDescription *pSubpasses,
  uint32_t dependencyCount,
  const VkSubpassDependency *pDependencies
);

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
);

VkResult wlu_create_pipeline_cache(vkcomp *app, size_t initialDataSize, const void *pInitialData);

VkResult wlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t cur_dd,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
);

/**
* Create the layouts that specify which resource (VkBuffer, VkImage)
* the shaders in the graphics pipeline will access
*/
VkResult wlu_create_desc_set_layouts(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
);

/* Inorder to allocate descriptor sets you must create a descriptor pool */
VkResult wlu_create_desc_pool(
  vkcomp *app,
  uint32_t cur_dd,
  VkDescriptorPoolCreateFlags flags,
  uint32_t psize,
  VkDescriptorPoolSize *pool_sizes
);

/**
* Function creates a descriptor set that one can use to
* manipulate the data contained in a VkBuffer (Uniform).
* Specifies actual resource (VkBuffer,VkImage) that will be
* bounded to the descriptors
*/
VkResult wlu_create_desc_set(
  vkcomp *app,
  uint32_t cur_dd
);

#endif
