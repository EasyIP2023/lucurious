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

#ifndef DLU_MATH_SET_H
#define DLU_MATH_SET_H

/**
* Usage of functions in set.h files is optional
* Added set functions as an extra options for those
* who don't want to fill out the entire VK struct themselves
*/

float dlu_set_radian(float fovy);

void dlu_set_perspective(mat4 proj, float fovy, float aspect, float nearVal, float farVal);

void dlu_set_lookat(mat4 view, vec3 eye, vec3 center, vec3 up);

void dlu_set_mvp_matrix(mat4 mvp, mat4 *clip, mat4 *proj, mat4 *view, mat4 *model);

void dlu_set_matrix(dlu_matrix_type type, void *dest, void *src);

void dlu_set_vector(dlu_vec_type type, void *dest, void *src);

void dlu_set_rotate(dlu_rotate_type type, void *dest, float rad, void *sr);

#endif
