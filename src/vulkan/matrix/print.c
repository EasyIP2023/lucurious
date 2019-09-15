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

void wlu_print_matrices(vkcomp *app) {
  wlu_log_me(WLU_INFO, "Perspective Matrix");
  wlu_log_me(WLU_INFO, "Projection from camera to screen");
  glm_mat4_print(app->proj, stdout);
  wlu_log_me(WLU_INFO, "View Matrix");
  wlu_log_me(WLU_INFO, "View from world space to camera space");
  glm_mat4_print(app->view, stdout);
  wlu_log_me(WLU_INFO, "Model Matrix");
  wlu_log_me(WLU_INFO, "Mapping object's local coordinate space into world space");
  glm_mat4_print(app->model, stdout);
  wlu_log_me(WLU_INFO, "Clip Matrix");
  glm_mat4_print(app->clip, stdout);
  wlu_log_me(WLU_INFO, "MVP Matrix");
  glm_mat4_print(app->mvp, stdout);
}

void wlu_print_vector(void *vector, wlu_vec_type type) {
  switch (type) {
    case WLU_VEC3:
      glm_vec3_print(*((vec3*) vector), stdout);
      break;
    case WLU_VEC4:
      glm_vec4_print(*((vec4*) vector), stdout);
      break;
    default:
      break;
  }
}
