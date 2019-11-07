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
#include <wlu/vlucur/gp.h>
#include <wlu/utils/log.h>

void wlu_update_descriptor_sets(
  vkcomp *app,
  uint32_t cur_dd,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  VkDescriptorType descriptorType,
  VkDescriptorBufferInfo *pBufferInfo,
  uint32_t psize
) {

  /* Copy Uniform Buffer Info */
  VkWriteDescriptorSet writes[psize];
  for (uint32_t i = 0; i < psize; i++) {
    writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[i].pNext = NULL;
    writes[i].dstSet = app->desc_data[cur_dd].desc_set[i];
    writes[i].dstBinding = dstBinding;
    writes[i].dstArrayElement = dstArrayElement;
    writes[i].descriptorCount = app->desc_data[cur_dd].dc;
    writes[i].descriptorType = descriptorType;
    writes[i].pImageInfo = NULL;
    writes[i].pBufferInfo = pBufferInfo;
    writes[i].pImageInfo = NULL;
  }

  vkUpdateDescriptorSets(app->device, psize, writes, 0, NULL);
}
