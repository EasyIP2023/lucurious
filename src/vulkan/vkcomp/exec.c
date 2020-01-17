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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>

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

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkEndCommandBuffer(app->cmd_data[cur_pool].cmd_buffs[i]);
    if (res) { PERR(WLU_VK_END_ERR, res, "CommandBuffer"); return res; }
  }

  return res;
}
