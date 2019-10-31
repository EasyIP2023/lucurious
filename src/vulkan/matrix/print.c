/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 EasyIP2023
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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/vlucur/matrix.h>
#include <wlu/utils/log.h>
#include <cglm/mat4.h>

void mat3_print(mat3 mat) {
  uint32_t mat_num = 3;

  wlu_log_me(WLU_WARNING, "%dx%d matrix:\n", mat_num, mat_num);

  for (uint32_t i = 0; i < mat_num; i++) {
    for (uint32_t j = 0; j < mat_num; j++)
      fprintf(stdout, "\x1B[30;1m" "|\t%0.4f\t", mat[i][j]);
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "\x1b[0m" "\n");
}

void mat4_print(mat4 mat) {
  uint32_t mat_num = 4;

  wlu_log_me(WLU_WARNING, "%dx%d matrix:\n", mat_num, mat_num);

  for (uint32_t i = 0; i < mat_num; i++) {
    for (uint32_t j = 0; j < mat_num; j++)
      fprintf(stdout, "\x1B[30;1m" "|\t%0.3f\t", mat[i][j]);
    fprintf(stdout, " |\n");
  }

  fprintf(stdout, "\x1b[0m" "\n");
}

void vec_print(vec2 vec, wlu_vec_type type) {
  int vec_num = type + 2;

  wlu_log_me(WLU_WARNING, "%dD vector:\n", vec_num);

  for (int i = 0; i < vec_num; i++)
    fprintf(stdout, "\x1B[30;1m" "|\t%0.3f\t", vec[i]);
  fprintf(stdout, "|\n");

  fprintf(stdout, "\x1b[0m" "\n");
}

void wlu_print_matrices(vkcomp *app) {
  wlu_log_me(WLU_INFO, "Perspective Matrix");
  wlu_log_me(WLU_INFO, "Projection from camera to screen");
  wlu_print_matrix(app->ubd.proj, WLU_MAT4);
  wlu_log_me(WLU_INFO, "View Matrix");
  wlu_log_me(WLU_INFO, "View from world space to camera space");
  wlu_print_matrix(app->ubd.view, WLU_MAT4);
  wlu_log_me(WLU_INFO, "Model Matrix");
  wlu_log_me(WLU_INFO, "Mapping object's local coordinate space into world space");
  wlu_print_matrix(app->ubd.model, WLU_MAT4);
  wlu_log_me(WLU_INFO, "Clip Matrix");
  wlu_print_matrix(app->ubd.clip, WLU_MAT4);
  wlu_log_me(WLU_INFO, "MVP Matrix");
  wlu_print_matrix(app->ubd.mvp, WLU_MAT4);
}

void wlu_print_vector(void *vector, wlu_vec_type type) {
  switch (type) {
    case WLU_VEC2: vec_print(*((vec2*) vector), type); break;
    case WLU_VEC3: vec_print(*((vec3*) vector), type); break;
    case WLU_VEC4: vec_print(*((vec4*) vector), type); break;
  }
}

void wlu_print_matrix(void *matrix, wlu_matrix_type type) {
  switch (type) {
    case WLU_MAT3: mat3_print(*((mat3*) matrix)); break;
    case WLU_MAT4: mat4_print(*((mat4*) matrix)); break;
  }
}
