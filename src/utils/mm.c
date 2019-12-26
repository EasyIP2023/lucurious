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
#include <sys/mman.h>

/**
* Struct that stores block metadata
* Using linked list to keep track of memory allocated
* next    | points to next memory block
* is_free | checks if memory block is free or not
* size    | allocated memory size
* saddr   | starting address for the block
*/
typedef struct mblock {
  struct mblock *next;
  bool is_free;
  size_t size;
  void *saddr;
} wlu_mem_block_t;

/* Keep track of last block starting address */
static void *last_addr = NULL;
static wlu_mem_block_t *mema_list = NULL;

#define BLOCK_SIZE sizeof(wlu_mem_block_t)
#define ALLOC_ERR (void*)-1

static wlu_mem_block_t *get_free_block(size_t bytes) {
	wlu_mem_block_t *current = mema_list;
	while (current) {
		if (current->is_free && current->size >= bytes)
			return current;
		current = current->next;
	}
	return NULL;
}

static wlu_mem_block_t *alloc_mem_block(size_t bytes) {
  /* change break point to get starting address for block of memory */
  wlu_mem_block_t *block = NULL;
  block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (block == ALLOC_ERR) {
    wlu_log_me(WLU_DANGER, "[x] mmap: %s", strerror(errno));
    return NULL;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = bytes;

  /* Want to start block address at an address that does not include metadata */
  block->saddr = block + BLOCK_SIZE;

  last_addr = block;
  return block;
}

void *wlu_alloc(size_t bytes) {
  wlu_mem_block_t *saddr = mema_list;
  wlu_mem_block_t *nblock = NULL;
  void *temp = last_addr;

  /* if free block found return block */
  nblock = get_free_block(bytes);
  if (nblock) return nblock;

  nblock = alloc_mem_block(bytes);
  if (!nblock) return NULL;

  if (!saddr) {
    /* This will create first link/block in linked list */
    saddr = nblock;
  } else {
    /* Retrieve last used memory block */
    mema_list = temp;
    mema_list->next = nblock;
  }

  /* reset memory address list to starting heap address */
  mema_list = saddr;
  return nblock->saddr;
}

/* Freeing memory in this case means to override the contents */
void wlu_free_block(void *addr) {
  wlu_mem_block_t *current = mema_list;
  while (current) {
    if (addr == current) { current->is_free = true; return; }
    current = current->next;
  }
}

void wlu_release_block(void *addr) {
  wlu_mem_block_t *current = mema_list;
  while (current) {
    if (addr == current) {
      if (munmap(current->saddr, BLOCK_SIZE+current->size) == -1)
        wlu_log_me(WLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    current = current->next;
  }
}

void wlu_print_mb() {
  wlu_mem_block_t *current = mema_list;
  while(current) {
    wlu_log_me(WLU_INFO, "current block = %p, next block = %p, isfree = %d, block size = %d, saddr = %p",
               current, current->next, current->is_free, current->size, current->saddr);
    current = current->next;
  }
}
