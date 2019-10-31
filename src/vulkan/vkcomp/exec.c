/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 EasyIP2023
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
  uint32_t cur_sc,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_pbs[cur_pool].cmd_buffs) {
    wlu_log_me(WLU_DANGER, "[x] app->cmd_pbs[%d].cmd_buffs must be initialize", cur_pool);
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_cmd_buffs(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.pNext = NULL;
  begin_info.flags = flags;
  begin_info.pInheritanceInfo = pInheritanceInfo;

  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    res = vkBeginCommandBuffer(app->cmd_pbs[cur_pool].cmd_buffs[i], &begin_info);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] Failed to start recording command buffer [%d], ERROR CODE: %d", i, res);
      return res;
    }
  }

  return res;
}

VkResult wlu_exec_stop_cmd_buffs(vkcomp *app, uint32_t cur_pool, uint32_t cur_sc) {
  VkResult res = VK_RESULT_MAX_ENUM;

  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    res = vkEndCommandBuffer(app->cmd_pbs[cur_pool].cmd_buffs[i]);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] Failed to stop recording command buffer [%d], ERROR CODE: %d", i, res);
      return res;
    }
  }

  return res;
}
