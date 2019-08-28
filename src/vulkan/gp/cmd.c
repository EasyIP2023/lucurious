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

void wlu_cmd_draw(
  vkcomp *app,
  uint32_t cur_buff,
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance
) {
  vkCmdDraw(app->cmd_buffs[cur_buff], vertexCount,
            instanceCount, firstVertex, firstInstance);
}

void wlu_cmd_set_viewport(
  vkcomp *app,
  VkViewport viewport,
  uint32_t cur_buff,
  uint32_t firstViewport,
  uint32_t viewportCount
) {
  app->viewport = viewport;
  vkCmdSetViewport(app->cmd_buffs[cur_buff], firstViewport,
                  viewportCount, &app->viewport);
}

void wlu_cmd_set_scissor(
  vkcomp *app,
  VkRect2D scissor,
  uint32_t cur_buff,
  uint32_t firstScissor,
  uint32_t scissorCount
) {
  app->scissor = scissor;
  vkCmdSetScissor(app->cmd_buffs[cur_buff], firstScissor,
                  scissorCount, &app->scissor);
}
