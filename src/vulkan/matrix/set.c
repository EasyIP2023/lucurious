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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/vlucur/matrix.h>
#include <wlu/utils/log.h>
#include <cglm/mat4.h>
#include <cglm/mat3.h>

float wlu_set_fovy(float fovy) {
  return glm_rad(fovy);
}

void wlu_set_perspective(
  vkcomp *app,
  float fovy,
  float aspect,
  float nearVal,
  float farVal
) {
  glmc_perspective(fovy, aspect, nearVal, farVal, app->proj);
}

void wlu_set_lookat(vkcomp *app, const float *eye, const float *center, const float *up) {
  vec3 eye2, center2, up2;
  memcpy(eye2, eye, sizeof(vec3));
  memcpy(center2, center, sizeof(vec3));
  memcpy(up2, up, sizeof(vec3));
  glm_lookat(eye2, center2, up2, app->view);
}

void wlu_set_matrix(mat4 *matrix, const void *model, wlu_matrix_type type) {
  switch (type) {
    case WLU_MAT3: memcpy(matrix, (mat3 *) model, sizeof(mat3)); break;
    case WLU_MAT4: memcpy(matrix, (mat4 *) model, sizeof(mat4)); break;
  }
}

void wlu_set_vector(void *vector, const float *vec, wlu_vec_type type) {
  switch (type) {
    case WLU_VEC2: memcpy(vector, vec, sizeof(vec2)); break;
    case WLU_VEC3: memcpy(vector, vec, sizeof(vec3)); break;
    case WLU_VEC4: memcpy(vector, vec, sizeof(vec4)); break;
  }
}

void wlu_set_mvp_matrix(vkcomp *app) {
  glm_mat4_mulN((mat4 *[]){&app->clip, &app->proj,
                &app->view, &app->model}, 4, app->mvp);
}
