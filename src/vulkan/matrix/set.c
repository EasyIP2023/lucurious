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
#include <cglm/call.h>

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

void wlu_set_lookat(vkcomp *app, vec3 eye, vec3 center, vec3 up) {
  glm_lookat(eye, center, up, app->view);
}

void wlu_set_matrix(void *matrix, void *model, wlu_matrix_type type) {
  switch (type) {
    case WLU_MAT3: memcpy((mat3 *) matrix, (mat3 *) model, sizeof(mat3)); break;
    case WLU_MAT4: memcpy((mat4 *) matrix, (mat4 *) model, sizeof(mat4)); break;
  }
}

void wlu_set_vector(void *vector, float *vec, wlu_vec_type type) {
  switch (type) {
    case WLU_VEC2: memcpy((vec2 *) vector, (vec2 *) vec, sizeof(vec2)); break;
    case WLU_VEC3: memcpy((vec3 *) vector, (vec3 *) vec, sizeof(vec3)); break;
    case WLU_VEC4: memcpy((vec4 *) vector, (vec4 *) vec, sizeof(vec4)); break;
  }
}

void wlu_set_mvp_matrix(vkcomp *app) {
  glm_mat4_mulN((mat4 *[]){&app->clip, &app->proj,
                &app->view, &app->model}, 4, app->mvp);
}
