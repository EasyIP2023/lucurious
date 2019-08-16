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

#ifndef SHADE_H
#define SHADE_H

#include <shaderc/shaderc.h>

typedef struct wlu_shader_info {
  const char *bytes;
  long byte_size;
} wlu_shader_info;

wlu_shader_info wlu_preprocess_shader(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name
);

wlu_shader_info wlu_compile_to_assembly(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
);

wlu_shader_info wlu_compile_to_spirv(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
);

#endif