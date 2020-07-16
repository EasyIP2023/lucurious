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

#ifndef DLU_VKCOMP_VK_CALLS_H
#define DLU_VKCOMP_VK_CALLS_H

/* Allows for vulkan synchronization function calling within lucurious */
VkResult dlu_vk_sync(dlu_sync_type type, vkcomp *app, uint32_t cur_scd, uint32_t synci);

/* Allows for more developer vulkan object destruction control */
void dlu_vk_destroy(dlu_destroy_type type, vkcomp *app, uint32_t cur_ld, void *data);

VkResult dlu_vk_map_mem(
  dlu_mem_map_type type,
  vkcomp *app,
  uint32_t cur_idx,
  VkDeviceSize size,
  void *data,
  VkDeviceSize offset,
  VkMemoryMapFlags flags
);

#endif