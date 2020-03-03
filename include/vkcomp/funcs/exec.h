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

#ifndef WLU_VKCOMP_EXEC_FUNCS_H
#define WLU_VKCOMP_EXEC_FUNCS_H

VkResult wlu_exec_begin_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
);

VkResult wlu_exec_stop_cmd_buffs(vkcomp *app, uint32_t cur_pool, uint32_t cur_scd);

VkResult wlu_exec_copy_buffer(
  vkcomp *app,
  uint32_t cur_pool,
  VkBuffer src_buffer,
  VkBuffer dst_buffer,
  VkDeviceSize size
);

/**
* Using image memory barrier to preform layout transitions.
* Used to synchronize access to resources. 
* Example: writing to a buffer completely before reading from it
*/
VkResult wlu_exec_transition_img_layout(
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

#endif
