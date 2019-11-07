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

const char fragShaderText[] =
  "#version 400\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "#extension GL_ARB_shading_language_420pack : enable\n"
  "layout (location = 0) in vec4 color;\n"
  "layout (location = 0) out vec4 outColor;\n"
  "void main() {\n"
  "   outColor = color;\n"
  "}";

const char vertShaderText[] =
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
  "}";

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

#endif
