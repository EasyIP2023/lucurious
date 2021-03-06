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

#ifndef DLU_UTILS_MM_H
#define DLU_UTILS_MM_H

/* [One Time Memory Allocater] For creating large memory blocks once */
bool dlu_otma(dlu_block_type type, dlu_otma_mems ma);
/* [One Time Buffer Allocater] For sub-allocating blocks of memory from large block */
bool dlu_otba(dlu_data_type type, void *addr, uint32_t index, uint32_t arr_size);

void dlu_release_blocks();

#ifdef DEV_ENV
void dlu_print_mb(dlu_block_type type);
#endif

#ifdef INAPI_CALLS
/* Function is reserve for one time use. Only used when allocating space for struct members */
void *dlu_alloc(dlu_block_type type, size_t bytes);
#endif

#endif
