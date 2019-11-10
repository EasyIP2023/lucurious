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
#include <wlu/utils/log.h>

/* https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer */
VkResult wlu_copy_buffer(
  vkcomp *app,
  uint32_t cur_pool,
  VkBuffer src_buffer,
  VkBuffer dst_buffer,
  VkDeviceSize size
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = app->cmd_data[cur_pool].cmd_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer cmd_buff;
  res = vkAllocateCommandBuffers(app->device, &alloc_info, &cmd_buff);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateCommandBuffers failed, ERROR CODE: %d", res);
    return res;
  }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  res = vkBeginCommandBuffer(cmd_buff, &begin_info);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkBeginCommandBuffer failed, ERROR CODE: %d", res);
    return res;
  }

  VkBufferCopy copy_region = {};
  copy_region.srcOffset = 0; // Optional
  copy_region.dstOffset = 0; // Optional
  copy_region.size = size;
  vkCmdCopyBuffer(cmd_buff, src_buffer, dst_buffer, 1, &copy_region);

  res = vkEndCommandBuffer(cmd_buff);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkEndCommandBuffer failed, ERROR CODE: %d", res);
    goto finish_cb_copy;
  }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buff;

  res = vkQueueSubmit(app->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkQueueSubmit failed, ERROR CODE: %d", res);
    goto finish_cb_copy;
  }

  res = vkQueueWaitIdle(app->graphics_queue);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkQueueWaitIdle failed, ERROR CODE: %d", res);
    goto finish_cb_copy;
  }

finish_cb_copy:
  vkFreeCommandBuffers(app->device, app->cmd_data[cur_pool].cmd_pool, 1, &cmd_buff);

  return res;
}
