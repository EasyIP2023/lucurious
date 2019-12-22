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

#include <lucom.h>
#include <wlu/utils/log.h>
#include <wlu/utils/mm.h>

#define BLOCK_SIZE sizeof(wlu_mem_block)
#define SBRK_ERR (void*)-1

static wlu_mem_block *alloc_mem_block(size_t size) {
  /* set first break point or starting address for block of memory */
  wlu_mem_block *block = (wlu_mem_block *) sbrk(0);

  /* set second break point or ending address for block of memory */
  void *alloc_mem = (void *) sbrk(BLOCK_SIZE * size);

  if (alloc_mem == SBRK_ERR) {
    wlu_log_me(WLU_DANGER, "[x] sbrk: %s", strerror(errno));
    return NULL;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = size;

  /* Want to start address at an address that does not include metadata */
  block->saddr = block + BLOCK_SIZE;

  return block;
}

void wlu_alloc(size_t size, wlu_mem_block **head) {
  wlu_mem_block *current = *head;

  if (!current) {
    *head = alloc_mem_block(size);
    if (!(*head)) return;
  } else {
    /* Retrieve last memory block */
    while (current->next) current = current->next;
    wlu_mem_block *nblock = alloc_mem_block(size);
    if (!nblock) return;
    current->next = nblock;
  }
}

/* Freeing memory means to override the contents */
void wlu_free(wlu_mem_block **head) {
  if (!(*head)) return;
  (*head)->is_free = true;
}

void wlu_print_mb(wlu_mem_block *current) {
  while(current) {
    wlu_log_me(WLU_INFO, "isfree = %d, size = %d, saddr = %p, current node = %p, next node = %p",
               current->is_free, current->size, current->saddr, current, current->next);
    current = current->next;
  }
}
