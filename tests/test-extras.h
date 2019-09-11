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
  "VK_LAYER_LUNARG_core_validation",
  "VK_LAYER_LUNARG_standard_validation",
  "VK_LAYER_KHRONOS_validation"
};

static const float pos_vertices[3][2] = {
  {0.0f, -0.5f},
  {0.5f, 0.5f},
  {-0.5f, 0.5f}
};

static const float color_vertices[3][3] = {
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f}
};

/* Can find in Vulkan SDK samples/API-Samples/data/cube_data.h */
typedef struct _vertex {
  float posX, posY, posZ, posW;  // Position data
  float r, g, b, a;              // Color
} vertex;

#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f

static const vertex g_vb_solid_face_colors_Data[] = {
  // red face
  {XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
  {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
  {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
  {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
  {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
  {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
  // green face
  {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
  {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
  {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
  {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
  {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
  {XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
  // blue face
  {XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
  {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
  {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
  {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
  {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
  {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f)},
  // yellow face
  {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
  {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
  {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
  {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
  {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
  {XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f)},
  // magenta face
  {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
  {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
  {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
  {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
  {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
  {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
  // cyan face
  {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
  {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
  {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
  {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
  {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
  {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
};
/* End of samples/API-Samples/data/cube_data.h */

#endif
