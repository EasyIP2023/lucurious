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

#ifndef WLU_VKCOMP_MATH_TYPES_H
#define WLU_VKCOMP_MATH_TYPES_H

typedef enum _wlu_rotate_type {
  WLU_X = 0x0000,
  WLU_Y = 0x0001,
  WLU_Z = 0x0002,
  WLU_AXIS_Z = 0x0003
} wlu_rotate_type;

typedef enum _wlu_vec_type {
  WLU_VEC2 = 0x0000,
  WLU_VEC3 = 0x0001,
  WLU_VEC4 = 0x0002
} wlu_vec_type;

typedef enum _wlu_matrix_type {
  WLU_MAT3 = 0x0000,
  WLU_MAT4 = 0x0001,
  WLU_MAT3_IDENTITY = 0x0002,
  WLU_MAT4_IDENTITY = 0x0003
} wlu_matrix_type;

#endif