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

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lucom.h>
#include <wlu/utils/log.h>

static const int ERR64 = -1;

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

/* Keep track of first/last block addresses */
static void *last_addr = NULL, *start_addr = NULL;
static wlu_mem_block_t *mema_list = NULL;

#define BLOCK_SIZE sizeof(wlu_mem_block_t)
#define ALLOC_ERR (void*)-1

/* Helps in ensuring one does not waste time in context switching */
static wlu_mem_block_t *get_free_block(size_t bytes) {
	wlu_mem_block_t *current = mema_list;
	while (current) {
    if (current->is_free && current->size >= bytes) {
      current->is_free = false;
      return current;
    }
    current = current->next;
	}
  return NULL;
}

static wlu_mem_block_t *alloc_mem_block(size_t bytes) {
  wlu_mem_block_t *block = NULL;
  int fd = 0;

  /* Allows for zeros to be written into values of bytes allocated */
  fd = open("/dev/zero", O_RDWR);
  if (fd == ERR64) {
    wlu_log_me(WLU_DANGER, "[x] open: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
  if (block == ALLOC_ERR) {
    wlu_log_me(WLU_DANGER, "[x] mmap: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = bytes;

  /* Want to start block address at an address that does not include metadata */
  block->saddr = BLOCK_SIZE + block;

  last_addr = block;
finish_alloc_mem_block:
  if (close(fd) == ERR64)
    wlu_log_me(WLU_DANGER, "[x] close: %s", strerror(errno));
  return block;
}

void *wlu_alloc(size_t bytes) {
  wlu_mem_block_t *nblock = NULL;
  void *temp = last_addr;

  /* if free block found return block */
  nblock = get_free_block(bytes);
  if (nblock) return nblock;

  nblock = alloc_mem_block(bytes);
  if (!nblock) return NULL;

  if (!start_addr) {
    /* This will create first link/block in linked list */
    start_addr = nblock;
  } else {
    /* Retrieve last used memory block */
    mema_list = temp;
    /* remember this is the best way to append to end of linked-list */
    mema_list->next = nblock;
  }

  /* reset memory address list to starting heap address */
  mema_list = start_addr;
  return nblock->saddr;
}

void *wlu_realloc(void *addr, size_t new_size) {
  wlu_mem_block_t *current = mema_list;

  while (current) {
    if (addr == current)
      return mremap(addr, BLOCK_SIZE + current->size,
                    BLOCK_SIZE + new_size, MREMAP_MAYMOVE);
    current = current->next;
  }

  return wlu_alloc(new_size);
}

/**
* Freeing memory in this case means to override the contents
* If memory address can't be accessed break
* Node is most likely the end of linked list
*/
void wlu_free_block(void *addr) {
  wlu_mem_block_t *current = mema_list;
  while (current) {
    if (!current->next) return;
    if (addr == current->saddr) {
      current->is_free = true;
      return;
    }
    current = current->next;
  }
}

/**
* Releasing memory in this case means to
* unmap all virtual pages (remove page tables)
*/
void wlu_release_blocks() {
  void *next_block = NULL;
  while (mema_list) {
    next_block = (!mema_list->next) ? NULL : mema_list->next;
    if (munmap(mema_list, BLOCK_SIZE + mema_list->size) == ERR64) {
      wlu_log_me(WLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    mema_list = next_block;
  }
  mema_list = start_addr = NULL;
}

void wlu_print_mb() {
  wlu_mem_block_t *current = mema_list;
  while(current) {
    wlu_log_me(WLU_INFO, "current block = %p, next block = %p, isfree = %d, block size = %d, saddr = %p",
               current, current->next, current->is_free, current->size, current->saddr);
    current = current->next;
  }
  mema_list = start_addr;
}
