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

#ifndef DLU_GP_CREATE_H
#define DLU_GP_CREATE_H

VkShaderModule dlu_create_shader_module(vkcomp *app, uint32_t cur_ld, char *code, size_t code_size);

VkResult dlu_create_render_pass(
  vkcomp *app,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  const VkAttachmentDescription *pAttachments,
  uint32_t subpassCount,
  const VkSubpassDescription *pSubpasses,
  uint32_t dependencyCount,
  const VkSubpassDependency *pDependencies
);

VkResult dlu_create_graphics_pipelines(
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

VkResult dlu_create_pipeline_cache(vkcomp *app, uint32_t cur_ld, size_t initialDataSize, const void *pInitialData);

VkResult dlu_create_pipeline_layout(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_gpd,
  uint32_t cur_dd,
  uint32_t pushConstantRangeCount,
  const VkPushConstantRange *pPushConstantRanges
);

/**
* Create the layouts that specify which resource (VkBuffer, VkImage, etc...)
* the shaders in the graphics pipeline will access
*/
VkResult dlu_create_desc_set_layout(
  vkcomp *app,
  uint32_t cur_dd,
  uint32_t cur_dl,
  VkDescriptorSetLayoutCreateInfo *desc_set_info
);

/* Inorder to allocate descriptor sets you must create a descriptor pool */
VkResult dlu_create_desc_pool(
  vkcomp *app,
  uint32_t cur_ld,
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
VkResult dlu_create_desc_sets(
  vkcomp *app,
  uint32_t cur_dd
);

/**
* Takes a VkImage object in memory and converts it to a texture
*/
VkResult dlu_create_texture_image(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_tex,
  VkImageCreateInfo *img_info,
  VkFlags requirements_mask
);

/**
* Concept of Sampling: https://www.tutorialspoint.com/dip/concept_of_sampling.htm
* Generally textures are accessed through samplers.
* Function creates samplers which apply filtering and transformations
* to compute the final color that is retrieved. The sampler is used in
* the shader to read colors from the texture.
*/
VkResult dlu_create_texture_sampler(vkcomp *app, uint32_t cur_tex, VkSamplerCreateInfo *sample_info);


#endif
