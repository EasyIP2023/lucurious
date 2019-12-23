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

/**
* Struct that stores block metadata
* next    | points to next memory block
* is_free | checks if memory block is free or not
* size    | allocated memory size
* saddr   | starting adress for the block
* eaddr   | ending adress for the block
*/
typedef struct _wlu_mem_block {
  struct _wlu_mem_block *next;
  bool is_free;
  size_t size;
  void *saddr;
  void *eaddr;
} wlu_mem_block;

#define BLOCK_SIZE sizeof(wlu_mem_block)
#define SBRK_ERR (void*)-1

static wlu_mem_block *mema_list = NULL;

/**
* The moment one tries to access addresses in newly
* allocated virtual area. The kernel automatically
* creates new physical pages
*/
static wlu_mem_block *alloc_mem_block(size_t bytes) {
  /* set first break point or starting address for block of memory */
  wlu_mem_block *block = (wlu_mem_block *) sbrk(0);
  if (block == SBRK_ERR) {
    wlu_log_me(WLU_DANGER, "[x] sbrk: %s", strerror(errno));
    return NULL;
  }

  /* set second break point or ending address for block of memory */
  void *alloc_mem = (void *) sbrk(BLOCK_SIZE * bytes);
  if (alloc_mem == SBRK_ERR) {
    wlu_log_me(WLU_DANGER, "[x] sbrk: %s", strerror(errno));
    return NULL;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = bytes;

  /* Want to start block address at an address that does not include metadata */
  block->saddr = block + BLOCK_SIZE;
  /* Could get 'eaddr' by doing operation: saddr + size */
  block->eaddr = alloc_mem;

  return block;
}

void *wlu_alloc(size_t bytes) {
  wlu_mem_block *saddr = mema_list;
  void *ret_addr = NULL;

  wlu_mem_block *nblock = alloc_mem_block(bytes);
  if (!nblock) return NULL;

  if (!saddr) {
    /* This will create first link/block in linked list */
    saddr = nblock;
    ret_addr = nblock->saddr;
  } else {
    /* Retrieve last memory block */
    while (mema_list->next) mema_list = mema_list->next;
    mema_list->next = nblock;
    ret_addr = nblock->saddr;
  }

  /* reset memory address list to starting heap address */
  mema_list = saddr;
  return ret_addr;
}

/* Freeing memory means to override the contents */
void wlu_free(void *addr) {
  wlu_mem_block *current = mema_list;
  while (current) {
    if (addr == current) { current->is_free = true; return; }
    current = current->next;
  }
}

void wlu_print_mb() {
  wlu_mem_block *current = mema_list;
  while(current) {
    wlu_log_me(WLU_INFO, "current block = %p, next block = %p, isfree = %d, block size = %d, saddr = %p",
               current, current->next, current->is_free, current->size, current->saddr);
    current = current->next;
  }
}
