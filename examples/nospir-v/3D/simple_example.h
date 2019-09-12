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

#ifndef SIMPLE_EXAMPLE_H
#define SIMPLE_EXAMPLE_H

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

const char shader_frag_src[] =
  "#version 400\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "#extension GL_ARB_shading_language_420pack : enable\n"
  "layout (location = 0) in vec4 color;\n"
  "layout (location = 0) out vec4 outColor;\n"
  "void main() {\n"
  "   outColor = color;\n"
  "}\n";

const char shader_vert_src[] =
  "#version 400\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "#extension GL_ARB_shading_language_420pack : enable\n"
  "layout (std140, binding = 0) uniform bufferVals {\n"
  "    mat4 mvp;\n"
  "} myBufferVals;\n"
  "layout (location = 0) in vec4 pos;\n"
  "layout (location = 1) in vec4 inColor;\n"
  "layout (location = 0) out vec4 outColor;\n"
  "void main() {\n"
  "   outColor = inColor;\n"
  "   gl_Position = myBufferVals.mvp * pos;\n"
  "}\n";

static const float dir[3] = {-5, 3, -10};
static const float eye[3] = {0, 0, 0};
static const float up[3] = {0, -1, 0};

static const float clip_matrix[4][4] = {
  { 1.0f, 0.0f, 0.0f, 0.0f },
  { 0.0f,-1.0f, 0.0f, 0.0f },
  { 0.0f, 0.0f, 0.5f, 0.0f },
  { 0.0f, 0.0f, 0.5f, 1.0f },
};

static const float model_matrix[4][4] = {
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f, 1.0f }
};

// posX, posY, posZ, posW
static const float pos3D_vertices[36][4] = {
  // Red Face
  {-1.f, -1.f, 1.f, 1.f}, {-1.f, 1.f, 1.f, 1.f},
  {1.f, -1.f, 1.f, 1.f}, {1.f, -1.f, 1.f, 1.f},
  {-1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f},
  // Green face
  {-1.f, -1.f, -1.f, 1.f}, {1.f, -1.f, -1.f, 1.f},
  {-1.f, 1.f, -1.f, 1.f}, {-1.f, 1.f, -1.f, 1.f},
  {1.f, -1.f, -1.f, 1.f}, {1.f, 1.f, -1.f, 1.f},
  // blue face
  {-1.f, 1.f, 1.f, 1.f}, {-1.f, -1.f, 1.f, 1.f},
  {-1.f, 1.f, -1.f, 1.f}, {-1.f, 1.f, -1.f, 1.f},
  {-1.f, -1.f, 1.f, 1.f}, {-1.f, -1.f, -1.f, 1.f},
  // yellow face
  {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, -1.f, 1.f},
  {1.f, -1.f, 1.f, 1.f}, {1.f, -1.f, 1.f, 1.f},
  {1.f, 1.f, -1.f, 1.f}, {1.f, -1.f, -1.f, 1.f},
  // magenta face
  {1.f, 1.f, 1.f, 1.f}, {-1.f, 1.f, 1.f, 1.f},
  {1.f, 1.f, -1.f, 1.f}, {1.f, 1.f, -1.f, 1.f},
  {-1.f, 1.f, 1.f, 1.f}, {-1.f, 1.f, -1.f, 1.f},
  // cyan face
  {1.f, -1.f, 1.f, 1.f}, {1.f, -1.f, -1.f, 1.f},
  {-1.f, -1.f, 1.f, 1.f}, {-1.f, -1.f, 1.f, 1.f},
  {1.f, -1.f, -1.f, 1.f}, {-1.f, -1.f, -1.f, 1.f}
};

// r, g, b, a colors
static const float color3D_vertices[36][4] = {
  // Red face
  {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f, 0.f, 1.f},
  {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f, 0.f, 1.f},
  {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f, 0.f, 1.f},
  // Green face
  {0.f, 1.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f},
  {0.f, 1.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f},
  {0.f, 1.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f},
  // blue face
  {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f, 1.f, 1.f},
  {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f, 1.f, 1.f},
  {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f, 1.f, 1.f},
  // yellow face
  {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f},
  {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f},
  {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f},
  // magenta face
  {1.f, 0.f, 1.f, 1.f}, {1.f, 0.f, 1.f, 1.f},
  {1.f, 0.f, 1.f, 1.f}, {1.f, 0.f, 1.f, 1.f},
  {1.f, 0.f, 1.f, 1.f}, {1.f, 0.f, 1.f, 1.f},
  // cyan face
  {0.f, 1.f, 1.f, 1.f}, {0.f, 1.f, 1.f, 1.f},
  {0.f, 1.f, 1.f, 1.f}, {0.f, 1.f, 1.f, 1.f},
  {0.f, 1.f, 1.f, 1.f}, {0.f, 1.f, 1.f, 1.f}
};

#endif
