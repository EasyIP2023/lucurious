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

#ifndef DLU_VKCOMP_EXEC_H
#define DLU_VKCOMP_EXEC_H

VkResult dlu_exec_begin_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
);

VkResult dlu_exec_stop_cmd_buffs(vkcomp *app, uint32_t cur_pool, uint32_t cur_scd);

/**
* cur_pool: Function uses one time command buffer allocate/submit
* src_bd: must be a valid VkBuffer
* dst_bd: must be a valid VkBuffer
*/
VkResult dlu_exec_copy_buffer(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t src_bd,
  uint32_t dst_bd,
  VkDeviceSize srcOffset,
  VkDeviceSize dstOffset,
  VkDeviceSize size
);

/**
* cur_pool: Function uses one time command buffer allocate/submit
* cur_bd: must be a valid VkBuffer that contains your image pixels
*/
VkResult dlu_exec_copy_buff_to_image(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_bd,
  uint32_t cur_tex,
  VkImageLayout dstImageLayout,
  uint32_t regionCount,
  const VkBufferImageCopy *pRegions
);

/**
* cur_pool: Function uses one time command buffer allocate/submit
* Using image memory barrier to preform layout transitions.
* Image memory barrier is used to synchronize access to image resources.
* Example: writing to a buffer completely before reading from it
*/
VkResult dlu_exec_pipeline_barrier(
  vkcomp *app, 
  uint32_t cur_pool,
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkDependencyFlags dependencyFlags,
  uint32_t memoryBarrierCount,
  const VkMemoryBarrier *pMemoryBarriers,
  uint32_t bufferMemoryBarrierCount,
  const VkBufferMemoryBarrier *pBufferMemoryBarriers,
  uint32_t imageMemoryBarrierCount,
  const VkImageMemoryBarrier *pImageMemoryBarriers
);

void dlu_exec_begin_render_pass(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  uint32_t cur_gpd,
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height,
  uint32_t clearValueCount,
  const VkClearValue *pClearValues,
  VkSubpassContents contents
);

void dlu_exec_stop_render_pass(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd
);

#endif
