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

#include <cglm/cglm.h>

float dlu_set_radian(float fovy) {
  return glm_rad(fovy);
}

void dlu_set_perspective(
  mat4 proj,
  float fovy,
  float aspect,
  float nearPlane,
  float farPlane
) {
  glm_perspective(fovy, aspect, nearPlane, farPlane, proj);
}

void dlu_set_lookat(mat4 view, vec3 eye, vec3 center, vec3 up) {
  glm_lookat(eye, center, up, view);
}

void dlu_set_mvp_matrix(mat4 mvp, mat4 *clip, mat4 *proj, mat4 *view, mat4 *model) {
  glm_mat4_mulN((mat4 *[]){clip, proj, view, model}, 4, mvp);
}

void dlu_set_matrix(dlu_matrix_type type, void *dest, void *src) {
  switch (type) {
    case DLU_MAT3: glm_mat3_copy(src, dest); break;
    case DLU_MAT4: glm_mat4_copy(src, dest); break;
    case DLU_MAT3_IDENTITY: glm_mat3_identity(dest); break;
    case DLU_MAT4_IDENTITY: glm_mat4_identity(dest); break;
    default: break;
  }
}

void dlu_set_vector(dlu_vec_type type, void *dest, void *src) {
  switch(type) {
    case DLU_VEC2:
      memmove(dest, src, sizeof(vec2)); /* cglm doesn't have a vec2 to copy func */
      break;
    case DLU_VEC3:
      glm_vec3_copy(src, dest);
      break;
    case DLU_VEC4:
      glm_vec4_copy(src, dest);
      break;
    default: break;
  }
}

void dlu_set_rotate(dlu_rotate_type type, void *dest, float rad, void *src) {
  switch (type) {
    case DLU_X: glm_rotate_x(src, rad, dest); break;
    case DLU_Y: glm_rotate_y(src, rad, dest); break;
    case DLU_Z: glm_rotate_z(src, rad, dest); break;
    case DLU_AXIS_Z: glm_rotate(dest, rad, src); break;
    default: break;
  }
}
