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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void wlu_freeup_pixels(void *pixels) {
  /* stbi_uc (Vincent Keep for references) */
  stbi_image_free((stbi_uc *) pixels);
}

VkResult wlu_load_texture_image(VkExtent3D *extent, void **pixels, const char *image_path) {
  VkResult res = VK_RESULT_MAX_ENUM;
  int width = 0, height = 0, channels = 0;

  /* First load the image */
  *pixels = stbi_load(image_path, &width, &height, &channels, STBI_rgb_alpha);
  if (!(*pixels)) {
    wlu_log_me(WLU_DANGER, "[x] stbi_load: failed to load texture image %s", image_path);
    return res;
  }

  extent->width = width;
  extent->height = height;
  extent->depth = channels;

  return res = VK_SUCCESS;
}

/* Can find in vulkan SDK API-Samples/utils/util.cpp */
bool memory_type_from_properties(vkcomp *app, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(app->physical_device, &memory_properties);

  /* Search memtypes to find first index with those properties */
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((typeBits & 1) == 1) {
      /* Type is available, does it match user properties */
      if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  /* No memory types matched, return failure */
  return false;
}
