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

#ifndef WLU_VKCOMP_UTILS_H
#define WLU_VKCOMP_UTILS_H

/* Free up stbi pixel data (freeup image that is loaded) */
void wlu_freeup_pixels(void *pixels);

/* Function loads an image into memory to later be copied into a buffer */
VkResult wlu_load_texture_image(VkExtent3D *extent, void **pixels, const char *image_path);

#ifdef INAPI_CALLS
/**
* Can find in vulkan SDK API-Samples/utils/util.cpp
* Used to find a suitable memory type
*/
bool memory_type_from_properties(
  vkcomp *app,
  uint32_t typeBits,
  VkFlags requirements_mask,
  uint32_t *typeIndex
);
#endif

#endif
