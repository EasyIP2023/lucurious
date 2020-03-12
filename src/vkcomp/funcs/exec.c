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

static VkCommandBuffer exec_begin_single_time_cmd_buff(vkcomp *app, uint32_t cur_pool) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = app->cmd_data[cur_pool].cmd_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer cmd_buff;
  res = vkAllocateCommandBuffers(app->device, &alloc_info, &cmd_buff);
  if (res) { PERR(WLU_VK_ALLOC_ERR, res, "CommandBuffers"); return VK_NULL_HANDLE; }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  res = vkBeginCommandBuffer(cmd_buff, &begin_info);
  if (res) {
    vkFreeCommandBuffers(app->device, app->cmd_data[cur_pool].cmd_pool, 1, &cmd_buff);
    PERR(WLU_VK_BEGIN_ERR, res, "CommandBuffer");
    return VK_NULL_HANDLE;
  }

  return cmd_buff;
}

static VkResult exec_end_single_time_cmd_buff(vkcomp *app, uint32_t cur_pool, VkCommandBuffer *cmd_buff) {
  VkResult res = VK_RESULT_MAX_ENUM;

  res = vkEndCommandBuffer(*cmd_buff);
  if (res) { PERR(WLU_VK_END_ERR, res, "CommandBuffer"); goto finish_estcb; }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = cmd_buff;

  res = vkQueueSubmit(app->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  if (res) { PERR(WLU_VK_QUEUE_ERR, res, "Submit"); goto finish_estcb; }

  res = vkQueueWaitIdle(app->graphics_queue);
  if (res) { PERR(WLU_VK_QUEUE_ERR, res, "WaitIdle"); goto finish_estcb; }

finish_estcb:
  vkFreeCommandBuffers(app->device, app->cmd_data[cur_pool].cmd_pool, 1, cmd_buff);

  return res;
}

VkResult wlu_exec_begin_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_buffs) { PERR(WLU_VKCOMP_CMD_BUFFS, 0, NULL); return res; }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.pNext = NULL;
  begin_info.flags = flags;
  begin_info.pInheritanceInfo = pInheritanceInfo;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkBeginCommandBuffer(app->cmd_data[cur_pool].cmd_buffs[i], &begin_info);
    if (res) { PERR(WLU_VK_BEGIN_ERR, res, "CommandBuffer"); return res; }
  }

  return res;
}

VkResult wlu_exec_stop_cmd_buffs(vkcomp *app, uint32_t cur_pool, uint32_t cur_scd) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_buffs) { PERR(WLU_VKCOMP_CMD_BUFFS, 0, NULL); return res; }

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkEndCommandBuffer(app->cmd_data[cur_pool].cmd_buffs[i]);
    if (res) { PERR(WLU_VK_END_ERR, res, "CommandBuffer"); return res; }
  }

  return res;
}

VkResult wlu_exec_copy_buffer(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t src_bd,
  uint32_t dst_bd,
  VkDeviceSize srcOffset,
  VkDeviceSize dstOffset,
  VkDeviceSize size
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandBuffer cmd_buff = exec_begin_single_time_cmd_buff(app, cur_pool);
  if (!cmd_buff) return res;

  VkBufferCopy copy_region = {};
  copy_region.srcOffset = srcOffset;
  copy_region.dstOffset = dstOffset;
  copy_region.size = size;

  vkCmdCopyBuffer(cmd_buff, app->buff_data[src_bd].buff, app->buff_data[dst_bd].buff, 1, &copy_region);

  res = exec_end_single_time_cmd_buff(app, cur_pool, &cmd_buff);
  if (res) return res;

  return res;
}

VkResult wlu_exec_copy_buff_to_image(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_bd,
  uint32_t cur_tex,
  VkImageLayout dstImageLayout,
  uint32_t regionCount,
  const VkBufferImageCopy *pRegions
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandBuffer cmd_buff = exec_begin_single_time_cmd_buff(app, cur_pool);
  if (!cmd_buff) return res;

  vkCmdCopyBufferToImage(cmd_buff, app->buff_data[cur_bd].buff,
                        app->text_data[cur_tex].image,
                        dstImageLayout, regionCount, pRegions);

  res = exec_end_single_time_cmd_buff(app, cur_pool, &cmd_buff);
  if (res) return res;

  return res;
}

VkResult wlu_exec_transition_image_layout(
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
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandBuffer cmd_buff = exec_begin_single_time_cmd_buff(app, cur_pool);
  if (!cmd_buff) return res;

  vkCmdPipelineBarrier(cmd_buff, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                       bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);

  res = exec_end_single_time_cmd_buff(app, cur_pool, &cmd_buff);
  if (res) return res;

  return res;
}
