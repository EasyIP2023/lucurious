/**
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

#ifndef TEST_SHADE_H
#define TEST_SHADE_H

const char shader_src[] =
  "#version 450 en\n"
  "void main() { int x = MY_DEFINE; }\n";

/* Used in test-triangle.c / test-square.c */
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
/* Used in test-triangle.c / test-square.c */

/* Used in test-cube.c */
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

const char fragShaderText[] =
  "#version 400\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "#extension GL_ARB_shading_language_420pack : enable\n"
  "layout (location = 0) in vec4 color;\n"
  "layout (location = 0) out vec4 outColor;\n"
  "void main() {\n"
  "   outColor = color;\n"
  "}";
/* Used in test-cube.c */

/* Used in test-spin-square.c */
const char spin_square_frag_text[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) in vec3 v_Color;\n"
  "layout(location = 0) out vec4 o_Color;\n"
  "void main() { o_Color = vec4(v_Color, 1.0); }";

const char spin_square_vert_text[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "#extension GL_ARB_shading_language_420pack : enable\n"
  "layout(set = 0, binding = 0) uniform UniformBufferObject {\n"
  "   mat4 model;\n"
  "   mat4 view;\n"
  "   mat4 proj;\n"
  "} ubo;\n"
  "layout(location = 0) in vec2 i_Position;\n"
  "layout(location = 1) in vec3 i_Color;\n"
  "layout(location = 0) out vec3 v_Color;\n"
  "void main() {\n"
  "   gl_Position = ubo.proj * ubo.view * ubo.model * vec4(i_Position, 0.0, 1.0);\n"
  "   v_Color = i_Color;\n"
  "}";
/* Used in test-spin-square.c */

#endif
