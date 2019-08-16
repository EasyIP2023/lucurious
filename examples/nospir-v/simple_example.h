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
  VK_KHR_DISPLAY_EXTENSION_NAME
};

const char shader_frag_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) in vec3 fragColor;\n"
  "layout(location = 0) out vec4 outColor;\n"
  "void main() { outColor = vec4(fragColor, 1.0); }";

const char shader_vert_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) out vec3 fragColor;\n"
  "vec2 positions[3] = vec2[](\n"
  "\tvec2(0.0, -0.5),\n"
  "\tvec2(0.5, 0.5),\n"
  "\tvec2(-0.5, 0.5)\n"
  ");\n"
  "vec3 colors[3] = vec3[](\n"
  "\tvec3(1.0, 0.0, 0.0),\n"
  "\tvec3(0.0, 1.0, 0.0),\n"
  "\tvec3(0.0, 0.0, 1.0)\n"
  ");\n"
  "void main() {\n"
  "\tgl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);\n"
  "\tfragColor = colors[gl_VertexIndex];\n"
  "}";

#endif
