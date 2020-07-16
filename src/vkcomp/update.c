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

VkWriteDescriptorSet dlu_write_desc_set(
  VkDescriptorSet dstSet,
  uint32_t dstBinding,
  uint32_t dstArrayElement,
  uint32_t descriptorCount,
  VkDescriptorType descriptorType,
  const VkDescriptorImageInfo *pImageInfo,
  const VkDescriptorBufferInfo *pBufferInfo,
  const VkBufferView *pTexelBufferView
) {

  VkWriteDescriptorSet write = {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.pNext = NULL;
  write.dstSet = dstSet;
  write.dstBinding = dstBinding;
  write.dstArrayElement = dstArrayElement;
  write.descriptorCount = descriptorCount;
  write.descriptorType = descriptorType;
  write.pImageInfo = pImageInfo;
  write.pBufferInfo = pBufferInfo;
  write.pTexelBufferView = pTexelBufferView;

  return write;
}

void dlu_update_desc_sets(
  VkDevice device,
  uint32_t descriptorWriteCount,
  const VkWriteDescriptorSet *pDescriptorWrites,
  uint32_t descriptorCopyCount,
  const VkCopyDescriptorSet *pDescriptorCopies
) {

  vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}
