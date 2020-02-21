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

#ifndef LUCOM_H
#define LUCOM_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define NEG_ONE -1

#define INAPI_CALLS
#include "lucurious.h"

#define POW2_DEC(pow) 1 << pow

/**
* http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/
* https://stackoverflow.com/questions/23235910/variadic-unused-function-macro
*/
#define UNUSED1(z) (void)(z)
#define UNUSED2(y,z) UNUSED1(y),UNUSED1(z)
#define UNUSED3(x,y,z) UNUSED1(x),UNUSED2(y,z)
#define UNUSED4(b,x,y,z) UNUSED2(b,x),UNUSED2(y,z)
#define UNUSED5(a,b,x,y,z) UNUSED2(a,b),UNUSED3(x,y,z)
#define UNUSED6(a,b,c,x,y,z) UNUSED3(a,b,c),UNUSED3(x,y,z)
#define UNUSED7(a,b,c,d,x,y,z) UNUSED4(a,b,c,d),UNUSED3(x,y,z)
#define UNUSED8(a,b,c,d,w,x,y,z) UNUSED4(a,b,c,d),UNUSED4(w,x,y,z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif
