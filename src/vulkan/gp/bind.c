/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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
#include <wlu/utils/log.h>

void wlu_bind_pipeline(
  vkcomp *app,
  uint32_t cur_buff,
  VkPipelineBindPoint pipelineBindPoint,
  VkPipeline pipeline
) {
  vkCmdBindPipeline(app->cmd_buffs[cur_buff], pipelineBindPoint, pipeline);
}

void wlu_bind_desc_set(
  vkcomp *app,
  uint32_t cur_buf,
  VkPipelineBindPoint pipelineBindPoint,
  uint32_t firstSet,
  uint32_t dynamicOffsetCount,
  const uint32_t *pDynamicOffsets
) {
  vkCmdBindDescriptorSets(app->cmd_buffs[cur_buf], pipelineBindPoint,
                          app->pipeline_layout, firstSet,
                          app->desc_count, app->desc_set,
                          dynamicOffsetCount, pDynamicOffsets);
}

void wlu_bind_vertex_buff_to_cmd_buffs(
  vkcomp *app,
  uint32_t cur_buf,
  uint32_t firstBinding,
  uint32_t bindingCount,
  const VkBuffer *pBuffers,
  const VkDeviceSize *offsets
) {
  vkCmdBindVertexBuffers(app->cmd_buffs[cur_buf],
                        firstBinding, bindingCount,
                        pBuffers, offsets);
}
