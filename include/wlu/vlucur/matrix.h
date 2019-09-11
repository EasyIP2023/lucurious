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

#ifndef MATRIX_H
#define MATRIX_H

float wlu_set_fovy(float fovy);

void wlu_set_perspective(
  vkcomp *app,
  float fovy,
  float aspect,
  float nearVal,
  float farVal
);

void wlu_set_lookat(vkcomp *app, float dir[3], float eye[3], float up[3]);

void wlu_set_model_matrix(vkcomp *app, float m);

void wlu_set_vec2_matrix(void *vector, const float vec[2]);

void wlu_set_vec3_matrix(void *vector, const float vec[2]);

void wlu_print_vec3(void *vector);

void wlu_set_clip_matrix(vkcomp *app, float model[4][4]);

void wlu_set_mvp_matrix(vkcomp *app);

void wlu_print_matrices(vkcomp *app);

#endif
