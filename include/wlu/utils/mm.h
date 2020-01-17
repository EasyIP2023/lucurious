/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#ifndef WLU_MM_H
#define WLU_MM_H

typedef struct _wlu_otma_mems {
  uint32_t inta_cnt;  /* int array count */
  uint32_t cha_cnt;   /* char array count */
  uint32_t fla_cnt;    /* float array count */
  uint32_t dba_cnt;   /* double array count */
  uint32_t vkext_props_cnt;
  uint32_t vkval_layer_cnt;
  uint32_t vkcomp_cnt;
  uint32_t wclient_cnt;
  uint32_t desc_cnt;  /* descriptor count */
  uint32_t gp_cnt;    /* Graphics pipelines count */
  uint32_t si_cnt;    /* swap chain image count */
  uint32_t scd_cnt;   /* swap chain data count */
  uint32_t gpd_cnt;   /* graphics pipeline data count */
  uint32_t cmdd_cnt;  /* command data count */
  uint32_t bd_cnt;    /* buffer data count */
  uint32_t dd_cnt;    /* descriptor data count */
} wlu_otma_mems;

/* [one time memory allocater] For creating large memory blocks once */
bool wlu_otma(wlu_otma_mems ma);
void wlu_release_block();

#ifdef DEV_ENV
void wlu_print_mb();
#endif

#ifdef INAPI_CALLS

typedef enum _wlu_block_type {
  WLU_LARGE_BLOCK = 0x00000000,
  WLU_SMALL_BLOCK = 0x00000001
} wlu_block_type;

void *wlu_alloc(wlu_block_type type, size_t bytes);
void *wlu_realloc(void *addr, size_t new_size);
void wlu_free_block(void *addr);

#endif

#endif
