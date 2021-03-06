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

#ifndef LUCURIOUS_H
#define LUCURIOUS_H

#define INDEX_IGNORE 1UL << 31
#define ARR_LEN(var) (sizeof(var) / sizeof(var[0]))
#define BYTE_ALIGN __attribute__((aligned))
#define OFFSET_ALIGN(var, align) while(var%align)var++;

/* Contains linear algebra types used throughout apps */
#include <cglm/types.h>

#include "utils/types.h"
#include "utils/all.h"

#ifdef LUCUR_SPIRV_API
#include "spirv/types.h"
#include "spirv/all.h"
#endif

#ifdef LUCUR_DISPLAY_API
#include "display/types.h"
#include "display/all.h"
#endif

#ifdef LUCUR_MATH_API
#include "math/types.h"
#include "math/all.h"
#endif

#ifdef LUCUR_VKCOMP_API
#include "vkcomp/types.h"
#include "vkcomp/all.h"
#endif
 
#ifdef LUCUR_STBI_API
#include "stb_image.h"
#endif

#ifdef LUCUR_KTX_KHR_API
#include <ktx.h>
#endif

#endif
