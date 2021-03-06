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

VkQueueFlagBits ret_qfambit(char *str) {
  if (!strcmp(str, "VK_QUEUE_GRAPHICS_BIT")) return VK_QUEUE_GRAPHICS_BIT;
  if (!strcmp(str, "VK_QUEUE_COMPUTE_BIT")) return VK_QUEUE_COMPUTE_BIT;
  if (!strcmp(str, "VK_QUEUE_TRANSFER_BIT")) return VK_QUEUE_TRANSFER_BIT;
  if (!strcmp(str, "VK_QUEUE_SPARSE_BINDING_BIT")) return VK_QUEUE_SPARSE_BINDING_BIT;
  if (!strcmp(str, "VK_QUEUE_PROTECTED_BIT")) return VK_QUEUE_PROTECTED_BIT;

  dlu_print_msg(DLU_DANGER, "[x] No correct option was selected returning VK_QUEUE_FLAG_BITS_MAX_ENUM\n");
  dlu_print_msg(DLU_DANGER, "[x] For a list of available flags go here\n");
  dlu_print_msg(DLU_DANGER, "[x] https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkQueueFlagBits.html\n\n");

  return VK_QUEUE_FLAG_BITS_MAX_ENUM;
}

VkPhysicalDeviceType ret_dtype(char *str) {
  if (!strcmp(str, "VK_PHYSICAL_DEVICE_TYPE_OTHER")) return VK_PHYSICAL_DEVICE_TYPE_OTHER;
  if (!strcmp(str, "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU")) return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
  if (!strcmp(str, "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU")) return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
  if (!strcmp(str, "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU")) return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
  if (!strcmp(str, "VK_PHYSICAL_DEVICE_TYPE_CPU")) return VK_PHYSICAL_DEVICE_TYPE_CPU;

  dlu_print_msg(DLU_DANGER, "[x] No correct option was selected returning VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM\n");
  dlu_print_msg(DLU_DANGER, "[x] For a list of available numbers go here, usage: [0-4]\n");
  dlu_print_msg(DLU_DANGER, "[x] https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceType.html\n\n");

  return VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
}

void help_message(const char *file) {
  dlu_print_msg(DLU_SUCCESS, "Usage: %s [options]\n", file);
  dlu_print_msg(DLU_WARNING, "Example: %s --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU\n", file);
  dlu_print_msg(DLU_INFO, "Options:\n");
  dlu_print_msg(DLU_DANGER, "\t-l, --pgvl                      "); dlu_print_msg(DLU_INFO, "\tPrint global validation layers\n");
  dlu_print_msg(DLU_DANGER, "\t-i, --pie                       "); dlu_print_msg(DLU_INFO, "\tPrint instance extenstion list\n");
  dlu_print_msg(DLU_DANGER, "\t-d, --pde=<VkPhysicalDeviceType>"); dlu_print_msg(DLU_INFO, "\tPrint device extenstion list\n");
  dlu_print_msg(DLU_DANGER, "\t    --display-info=<drm device> "); dlu_print_msg(DLU_INFO, "\tDisplay compatible DRM Device and it's capabilities\n");
  dlu_print_msg(DLU_DANGER, "\t-v, --version                   "); dlu_print_msg(DLU_INFO, "\tPrint lucurious library version\n");
  dlu_print_msg(DLU_DANGER, "\t-h, --help                      "); dlu_print_msg(DLU_INFO, "\tShow this message\n");
}

void version_num() {
  dlu_print_msg(DLU_SUCCESS, "lucurious %s\n", LUCUR_VERSION);
}

void lower_to_upper(char *s) {
  while (*s != '\0') {
    if (*s >= 'a' && *s <= 'z')
      *s = ('A' + *s - 'a');
    s++;
  }
}
