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

#ifndef TEST_EXTRAS_H
#define TEST_EXTRAS_H

const char *device_extensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char *instance_extensions[] = {
  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  VK_KHR_SURFACE_EXTENSION_NAME,
  VK_KHR_DISPLAY_EXTENSION_NAME,
  VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

/* All of the useful standard validation is
  bundled into a layer included in the SDK */
const char *enabled_validation_layers[] = {
  "VK_LAYER_KHRONOS_validation"
};

/* used in test-triangle */
float pos_vertices[3][2] = {
  {0.0f, -0.5f},
  {0.5f, 0.5f},
  {-0.5f, 0.5f}
};

float color_vertices[3][3] = {
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f}
};
/* used in test-triangle */

/* used in test-cube */
vec3 eye = {-5, 3, -10};
vec3 center = {0, 0, 0};
vec3 up = {0, -1, 0};

mat4 clip_matrix = {
  { 1.0f, 0.0f, 0.0f, 0.0f },
  { 0.0f,-1.0f, 0.0f, 0.0f },
  { 0.0f, 0.0f, 0.5f, 0.0f },
  { 0.0f, 0.0f, 0.5f, 1.0f },
};

mat4 model_matrix = {
  { 1.0f, 0.0f, 0.0f, 0.0f },
  { 0.0f, 1.0f, 0.0f, 0.0f },
  { 0.0f, 0.0f, 1.0f, 0.0f },
  { 0.0f, 0.0f, 0.0f, 1.0f }
};

// { position }, { color }
vertex_3D vertices[36] = {
  // red face
  { {-1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  // green face
  { {-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { {-1.0f, 1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { {-1.0f, 1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, 1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  // blue face
  { {-1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  // yellow face
  { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  { {1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f} },
  // magenta face
  { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  { {-1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f} },
  // cyan face
  { {1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} },
  { {-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f} }
};
/* used in test-cube */

/* used in test-square */
uint16_t indices[6] = {0, 1, 2, 2, 3, 0};
/* used in test-square */

#endif
