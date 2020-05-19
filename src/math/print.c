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

#define LUCUR_MATH_API
#include <lucom.h>

#include <cglm/mat4.h>

void mat3_print(mat3 mat) {
  uint32_t mat_num = 3;

  dlu_log_me(DLU_WARNING, "%dx%d matrix:\n", mat_num, mat_num);

  for (uint32_t i = 0; i < mat_num; i++) {
    for (uint32_t j = 0; j < mat_num; j++)
      fprintf(stdout, "\x1B[30;1m" "|\t%0.4f\t", mat[i][j]);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "\x1b[0m" "\n");
}

void mat4_print(mat4 mat) {
  uint32_t mat_num = 4;

  dlu_log_me(DLU_WARNING, "%dx%d matrix:\n", mat_num, mat_num);

  for (uint32_t i = 0; i < mat_num; i++) {
    for (uint32_t j = 0; j < mat_num; j++)
      fprintf(stdout, "\x1B[30;1m" "|\t%0.3f\t", mat[i][j]);
    fprintf(stdout, " |\n");
  }

  fprintf(stdout, "\x1b[0m" "\n");
}

void vec_print(vec2 vec, dlu_vec_type type) {
  int vec_num = type + 2;

  dlu_log_me(DLU_WARNING, "%dD vector:\n", vec_num);

  for (int i = 0; i < vec_num; i++)
    fprintf(stdout, "\x1B[30;1m" "|\t%0.3f\t", vec[i]);
  fprintf(stdout, "|\n");

  fprintf(stdout, "\x1b[0m" "\n");
}

void dlu_print_vector(dlu_vec_type type, void *vector) {
  switch (type) {
    case DLU_VEC2: vec_print(*((vec2*) vector), type); break;
    case DLU_VEC3: vec_print(*((vec3*) vector), type); break;
    case DLU_VEC4: vec_print(*((vec4*) vector), type); break;
    default: break;
  }
}

void dlu_print_matrix(dlu_matrix_type type, void *matrix) {
  switch (type) {
    case DLU_MAT3: mat3_print(*((mat3*) matrix)); break;
    case DLU_MAT4: mat4_print(*((mat4*) matrix)); break;
    default: break;
  }
}
