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

#ifndef DLU_VKCOMP_GP_UPDATE_H
#define DLU_VKCOMP_GP_UPDATE_H

/* descriptorCount: Specify the amount of descriptors to update */
VkWriteDescriptorSet dlu_write_desc_set(
  VkDescriptorSet dstSet,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  uint32_t descriptorCount,
  VkDescriptorType descriptorType,
  const VkDescriptorImageInfo *pImageInfo,
  const VkDescriptorBufferInfo *pBufferInfo,
  const VkBufferView *pTexelBufferView
);

/* Update the configurations of descriptors */
void dlu_update_desc_sets(
  vkcomp *app,
  uint32_t descriptorWriteCount,
  const VkWriteDescriptorSet *pDescriptorWrites,
  uint32_t descriptorCopyCount,
  const VkCopyDescriptorSet *pDescriptorCopies
);

#endif
