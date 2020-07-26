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

VkCommandBuffer dlu_exec_begin_single_time_cmd_buff(vkcomp *app, uint32_t cur_pool) {
  VkCommandBuffer cmd_buff = VK_NULL_HANDLE;
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_pool) { PERR(DLU_VKCOMP_CMD_POOL, 0, NULL); return cmd_buff; }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = app->cmd_data[cur_pool].cmd_pool;
  alloc_info.commandBufferCount = 1;

  res = vkAllocateCommandBuffers(app->ld_data[app->cmd_data[cur_pool].ldi].device, &alloc_info, &cmd_buff);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkAllocateCommandBuffers"); return cmd_buff; }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  res = vkBeginCommandBuffer(cmd_buff, &begin_info);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkBeginCommandBuffer"); goto finish_estcb; }

  return cmd_buff;

finish_estcb:
  vkFreeCommandBuffers(app->ld_data[app->cmd_data[cur_pool].ldi].device, app->cmd_data[cur_pool].cmd_pool, 1, &cmd_buff);
  return VK_NULL_HANDLE;
}

VkResult dlu_exec_end_single_time_cmd_buff(vkcomp *app, uint32_t cur_pool, VkCommandBuffer *cmd_buff) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_pool) { PERR(DLU_VKCOMP_CMD_POOL, 0, NULL); return res; }
  
  res = vkEndCommandBuffer(*cmd_buff);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEndCommandBuffer"); goto finish_estcb; }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = cmd_buff;

  res = vkQueueSubmit(app->ld_data[app->cmd_data[cur_pool].ldi].graphics, 1, &submit_info, VK_NULL_HANDLE);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkQueueSubmit"); goto finish_estcb; }

  res = vkQueueWaitIdle(app->ld_data[app->cmd_data[cur_pool].ldi].graphics);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkQueueWaitIdle"); goto finish_estcb; }

finish_estcb:
  vkFreeCommandBuffers(app->ld_data[app->cmd_data[cur_pool].ldi].device, app->cmd_data[cur_pool].cmd_pool, 1, cmd_buff);

  return res;
}

void dlu_exec_copy_buffer(
  vkcomp *app,
  uint32_t src_bd,
  uint32_t dst_bd,
  VkDeviceSize srcOffset,
  VkDeviceSize dstOffset,
  VkDeviceSize size,
  VkCommandBuffer cmd_buff
) {

  VkBufferCopy copy_region = {};
  copy_region.srcOffset = srcOffset;
  copy_region.dstOffset = dstOffset;
  copy_region.size = size;

  vkCmdCopyBuffer(cmd_buff, app->buff_data[src_bd].buff, app->buff_data[dst_bd].buff, 1, &copy_region);
}

void dlu_exec_copy_buff_to_image(
  vkcomp *app,
  uint32_t cur_bd,
  uint32_t cur_tex,
  VkImageLayout dstImageLayout,
  uint32_t regionCount,
  const VkBufferImageCopy *pRegions,
  VkCommandBuffer cmd_buff
) {

  vkCmdCopyBufferToImage(cmd_buff, app->buff_data[cur_bd].buff, app->text_data[cur_tex].image,
                        dstImageLayout, regionCount, pRegions);
}

void dlu_exec_pipeline_barrier(
  VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask,
  VkDependencyFlags dependencyFlags,
  uint32_t memoryBarrierCount,
  const VkMemoryBarrier *pMemoryBarriers,
  uint32_t bufferMemoryBarrierCount,
  const VkBufferMemoryBarrier *pBufferMemoryBarriers,
  uint32_t imageMemoryBarrierCount,
  const VkImageMemoryBarrier *pImageMemoryBarriers,
  VkCommandBuffer cmd_buff
) {

  vkCmdPipelineBarrier(cmd_buff, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                       bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

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
) {

  if (!app->sc_data[cur_scd].sc_buffs) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA_MEMS"); return; }

  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.pNext = NULL;
  render_pass_info.renderPass = app->gp_data[cur_gpd].render_pass;
  render_pass_info.renderArea.offset.x = x;
  render_pass_info.renderArea.offset.y = y;
  render_pass_info.renderArea.extent.width = width;
  render_pass_info.renderArea.extent.height = height;
  render_pass_info.clearValueCount = clearValueCount;
  render_pass_info.pClearValues = pClearValues;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    render_pass_info.framebuffer = app->sc_data[cur_scd].sc_buffs[i].fb;
    /* Instert render pass into command buffer */
    vkCmdBeginRenderPass(app->cmd_data[cur_pool].cmd_buffs[i], &render_pass_info, contents);
  }
}

void dlu_exec_stop_render_pass(vkcomp *app, uint32_t cur_pool, uint32_t cur_scd) {
  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++)
    vkCmdEndRenderPass(app->cmd_data[cur_pool].cmd_buffs[i]);
}

VkResult dlu_exec_begin_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_buffs) { PERR(DLU_VKCOMP_CMD_BUFFS, 0, NULL); return res; }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.pNext = NULL;
  begin_info.flags = flags;
  begin_info.pInheritanceInfo = pInheritanceInfo;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkBeginCommandBuffer(app->cmd_data[cur_pool].cmd_buffs[i], &begin_info);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkBeginCommandBuffer"); return res; }
  }

  return res;
}

VkResult dlu_exec_stop_cmd_buffs(vkcomp *app, uint32_t cur_pool, uint32_t cur_scd) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_data[cur_pool].cmd_buffs) { PERR(DLU_VKCOMP_CMD_BUFFS, 0, NULL); return res; }

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkEndCommandBuffer(app->cmd_data[cur_pool].cmd_buffs[i]);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEndCommandBuffer"); return res; }
  }

  return res;
}

void dlu_exec_cmd_draw(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance
) {

  vkCmdDraw(app->cmd_data[cur_pool].cmd_buffs[cur_buff], vertexCount, instanceCount, firstVertex, firstInstance);
}

void dlu_exec_cmd_draw_indexed(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t indexCount,
  uint32_t instanceCount,
  uint32_t firstIndex,
  int32_t vertexOffset,
  uint32_t firstInstance
) {

  vkCmdDrawIndexed(app->cmd_data[cur_pool].cmd_buffs[cur_buff], indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void dlu_exec_cmd_set_viewport(
  vkcomp *app,
  VkViewport *viewport,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t firstViewport,
  uint32_t viewportCount
) {

  vkCmdSetViewport(app->cmd_data[cur_pool].cmd_buffs[cur_buff], firstViewport, viewportCount, viewport);
}

void dlu_exec_cmd_set_scissor(
  vkcomp *app,
  VkRect2D *scissor,
  uint32_t cur_pool,
  uint32_t cur_buff,
  uint32_t firstScissor,
  uint32_t scissorCount
) {

  vkCmdSetScissor(app->cmd_data[cur_pool].cmd_buffs[cur_buff], firstScissor, scissorCount, scissor);
}
