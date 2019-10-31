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

#ifndef MATRIX_H
#define MATRIX_H

typedef enum wlu_vec_type {
  WLU_VEC2 = 0,
  WLU_VEC3 = 1,
  WLU_VEC4 = 2
} wlu_vec_type;

typedef enum wlu_matrix_type {
  WLU_MAT3 = 0,
  WLU_MAT4 = 1
} wlu_matrix_type;

float wlu_set_fovy(float fovy);

void wlu_set_perspective(
  vkcomp *app,
  float fovy,
  float aspect,
  float nearVal,
  float farVal
);

void wlu_set_lookat(vkcomp *app, vec3 eye, vec3 center, vec3 up);

void *wlu_set_matrix(void *matrix, void *model, uint32_t size);

void *wlu_set_vector(void *vector, float *vec, uint32_t size);

void wlu_set_mvp_matrix(vkcomp *app);

void wlu_print_matrices(vkcomp *app);

void wlu_print_vector(void *vector, wlu_vec_type type);

void wlu_print_matrix(void *matrix, wlu_matrix_type type);

#endif
