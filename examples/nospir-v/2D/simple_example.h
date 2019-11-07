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

const char shader_frag_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) in vec3 v_Color;\n"
  "layout(location = 0) out vec4 o_Color;\n"
  "void main() { o_Color = vec4(v_Color, 1.0); }";

const char shader_vert_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "out gl_PerVertex {\n"
  "   vec4 gl_Position;\n"
  "};\n"
  "layout(location = 0) in vec2 i_Position;\n"
  "layout(location = 1) in vec3 i_Color;\n"
  "layout(location = 0) out vec3 v_Color;\n"
  "void main() {\n"
  "   gl_Position = vec4(i_Position, 0.0, 1.0);\n"
  "   v_Color = i_Color;\n"
  "}";

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

#endif
