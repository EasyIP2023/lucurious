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
#include "../../include/vkcomp/types.h"
#include "../../include/wayland/types.h"

/**
* Struct that stores block metadata
* Using linked list to keep track of memory allocated
* next     | points to next memory block
* is_free  | checks if memory block is free or not
* size     | allocated memory size
* abytes   | available bytes left in block
* addr     | Current address of the block
* saddr    | Starting address of the block where data is assigned
* prv_addr | Address of the previous block
*/
typedef struct mblock {
  struct mblock *next;
  bool is_free;
  size_t size;
  size_t abytes;
  void *addr;
  void *saddr;
  void *prv_addr;
} wlu_mem_block_t;

#define BLOCK_SIZE sizeof(wlu_mem_block_t)

/**
* Globals used to keep track of memory blocks
* sstart_addr_priv: Keep track of first allocated private small block address
* large_block_priv: A struct to keep track of one large allocated private block
* small_block_priv: A linked list for smaller allocated private block
* sstart_addr_shared: Keep track of first allocated small shared block address
* large_block_shared: A struct to keep track of one large allocated shared block
* small_block_shared: linked list for smaller allocated shared blocks
*/
static void *sstart_addr_priv = NULL;
static wlu_mem_block_t *large_block_priv = NULL;
static wlu_mem_block_t *small_block_priv = NULL;

static void *sstart_addr_shared = NULL;
static wlu_mem_block_t *large_block_shared = NULL;
static wlu_mem_block_t *small_block_shared = NULL;

/**
* Helps in ensuring one does not waste cycles in context switching
* First check if sub-block was allocated and is currently free
* If block not free, sub allocate more from larger memory block
*/
static wlu_mem_block_t *get_free_block(wlu_block_type type, size_t bytes, int fd) {
  wlu_mem_block_t *current = NULL;
  size_t abytes = 0;

  abytes = (type == WLU_SMALL_BLOCK_SHARED) ? large_block_shared->abytes : large_block_priv->abytes;
  current = (type == WLU_SMALL_BLOCK_SHARED) ? sstart_addr_shared : sstart_addr_priv;

  while (current->next) {
    if (current->is_free && current->size >= bytes) {
      current->is_free = false;
      return current;
    }
    current = current->next;
	}

  if (abytes >= bytes) {
    wlu_mem_block_t *block = current->addr;

    block->next = NULL;
    block->is_free = false;
    block->size = bytes;

    /* Put saddr at an address that doesn't contain metadata */
    block->saddr = BLOCK_SIZE + current->addr;

    /**
    * This is written this way becuase one needs to return the
    * address of the start of the next block not the current one.
    * Offset memory address effectively creating space
    */
    block = current->addr + BLOCK_SIZE + bytes;
    block->prv_addr = current->addr;
    block->addr = block;

    if (fd != NEG_ONE) {
      /* Allocate 32KB, done at compile time */
      char buff[POW2_DEC(15)];
      /* read contents from the wayland file into buff */
      if (read(fd, buff, bytes) == NEG_ONE) {
        wlu_log_me(WLU_DANGER, "[x] read: %s", strerror(errno));
        return NULL;
      }

      /* then memmove contents from the buff (which is a wayland file) into the block */
      if (!memmove(block + BLOCK_SIZE, buff, bytes)) {
        wlu_log_me(WLU_DANGER, "[x] memmove failed: Failed to copy file content");
        return NULL;
      }
    }

    /* Decrement larger block available memory */
    if (type == WLU_SMALL_BLOCK_SHARED) {
      large_block_shared->abytes -= (BLOCK_SIZE + bytes);
    } else {
      large_block_priv->abytes -= (BLOCK_SIZE + bytes);
    }

    return block;
  }

  return NULL;
}

static wlu_mem_block_t *alloc_mem_block(wlu_block_type type, size_t bytes) {
  wlu_mem_block_t *block = NULL;

  /* Allows for zeros to be written into values of bytes allocated */
  int fd = open("/dev/zero", O_RDWR);
  if (fd == NEG_ONE) {
    wlu_log_me(WLU_DANGER, "[x] open: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  switch (type) {
    case WLU_LARGE_BLOCK_PRIV:
      block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, fd, 0);
      if (block == MAP_FAILED) {
        wlu_log_me(WLU_DANGER, "[x] mmap: %s", strerror(errno));
        goto finish_alloc_mem_block;
      }
      break;
    case WLU_LARGE_BLOCK_SHARED:
      block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, fd, 0);
      if (block == MAP_FAILED) {
        wlu_log_me(WLU_DANGER, "[x] mmap: %s", strerror(errno));
        goto finish_alloc_mem_block;
      }
      break;
    default: break;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = block->abytes = bytes;

  /* Put saddr at an address that doesn't contain metadata */
  block->addr = block;
  block->saddr = BLOCK_SIZE + block;
finish_alloc_mem_block:
  if (close(fd) == NEG_ONE)
    wlu_log_me(WLU_DANGER, "[x] close: %s", strerror(errno));
  return block;
}

void *wlu_alloc(wlu_block_type type, size_t bytes, int fd) {
  wlu_mem_block_t *nblock = NULL;

  /**
  * This will create large block of memory
  * Then create a linked list of smaller blocks from the larger one
  * Retrieve last used memory block
  * O(1) appending to end of linked-list
  * reset linked list to starting address of linked list
  */
  /* TODO: Possible rewrite of switch statement */
  switch (type) {
    case WLU_LARGE_BLOCK_PRIV:
      /* If large block allocated don't allocate another one */
      if (large_block_priv) { PERR(WLU_ALREADY_ALLOC, 0, NULL); return NULL; }

      nblock = alloc_mem_block(type, bytes);
      if (!nblock) return NULL;
      large_block_priv = nblock;

      /**
      * Set small block allocation addr to address that
      * doesn't include larger block metadata
      */
      small_block_priv = sstart_addr_priv = nblock->saddr;
      small_block_priv->addr = small_block_priv;
      break;
    case WLU_SMALL_BLOCK_PRIV:
      /* If large block not allocated return NULL until allocated */
      if (!large_block_priv) return NULL;

      nblock = get_free_block(type, bytes, fd);
      if (!nblock) return NULL;

      /* set small block list at current addr of the last block in the list */
      small_block_priv = nblock->prv_addr;
      small_block_priv->next = nblock;

      /* Move back to previous block (for return status) */
      nblock = small_block_priv;
      small_block_priv = sstart_addr_priv; /* RESET */
      break;
    case WLU_LARGE_BLOCK_SHARED:
      /* If large block allocated don't allocate another one */
      if (large_block_shared) { PERR(WLU_ALREADY_ALLOC, 0, NULL); return NULL; }

      nblock = alloc_mem_block(type, bytes);
      if (!nblock) return NULL;
      large_block_shared = nblock;

      /**
      * Set small block allocation addr to address that
      * doesn't include larger block metadata
      */
      small_block_shared = sstart_addr_shared = nblock->saddr;
      small_block_shared->addr = small_block_shared;
      break;
    case WLU_SMALL_BLOCK_SHARED:
      /* If large block not allocated return NULL until allocated */
      if (!large_block_shared) return NULL;

      nblock = get_free_block(type, bytes, fd);
      if (!nblock) return NULL;

      /* set small block list at current addr of the last block in the list */
      small_block_shared = nblock->prv_addr;
      small_block_shared->next = nblock;

      /* Move back to previous block (for return status) */
      nblock = small_block_shared;
      small_block_priv = sstart_addr_shared; /* RESET */
      break;
    default: break;
  }

  return nblock->saddr;
}

bool wlu_otma(wlu_block_type type, wlu_otma_mems ma) {
  size_t size = 0;

  if (type == WLU_SMALL_BLOCK_PRIV || type == WLU_SMALL_BLOCK_SHARED) {
    PERR(WLU_OP_NOT_PERMITED, 0, NULL);
    return false;
  }

  if (large_block_priv) { PERR(WLU_ALREADY_ALLOC, 0, NULL); return false; }
  if (large_block_shared) { PERR(WLU_ALREADY_ALLOC, 0, NULL); return false; }

  size += (ma.inta_cnt * sizeof(int));
  size += (ma.cha_cnt * sizeof(char)); /* sizeof(char) is for formality */
  size += (ma.fla_cnt * sizeof(float));
  size += (ma.dba_cnt * sizeof(double));

  size += (ma.wclient_cnt * sizeof(struct _wlu_way_core));

  size += (ma.vkcomp_cnt * sizeof(vkcomp));
  size += (ma.vkext_props_cnt * sizeof(VkExtensionProperties));
  size += (ma.vkval_layer_cnt * sizeof(VkLayerProperties));

  size += (ma.si_cnt * sizeof(struct _swap_chain_buffers));
  size += (ma.si_cnt * sizeof(struct _semaphores));
  size += (ma.scd_cnt * sizeof(struct _sc_data));

  size += (ma.gp_cnt * sizeof(VkPipeline));
  size += (ma.gpd_cnt * sizeof(struct _gp_data));

  size += (ma.si_cnt * sizeof(VkCommandBuffer));
  size += (ma.cmdd_cnt * sizeof(struct _cmd_data));

  size += (ma.bd_cnt * sizeof(struct _buffs_data));

  size += (ma.desc_cnt * sizeof(VkDescriptorSet));
  size += (ma.desc_cnt * sizeof(VkDescriptorSetLayout));
  size += (ma.dd_cnt * sizeof(struct _desc_data));

  if (!wlu_alloc(type, size, NEG_ONE)) return false;

  return true;
}

/**
* Freeing memory in this case means to override the contents
* If memory address can't be accessed break
* Node is most likely the end of linked list
*/
void wlu_free_block(wlu_block_type type, void *addr) {
  wlu_mem_block_t *current = (type == WLU_SMALL_BLOCK_SHARED) ? sstart_addr_shared : sstart_addr_priv;
  while (current->next) {
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
  if (large_block_priv) {
    if (munmap(large_block_priv, BLOCK_SIZE + large_block_priv->size) == NEG_ONE) {
      wlu_log_me(WLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    large_block_priv = NULL;
  }
  if (large_block_shared) {
    if (munmap(large_block_shared, BLOCK_SIZE + large_block_shared->size) == NEG_ONE) {
      wlu_log_me(WLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    large_block_shared = NULL;
  }
}

void wlu_print_mb(wlu_block_type type) {
  wlu_mem_block_t *current = (type == WLU_SMALL_BLOCK_SHARED) ? sstart_addr_shared : sstart_addr_priv;
  while (current->next) {
    wlu_log_me(WLU_INFO, "current block = %p, next block = %p, isfree = %d, block size = %d, saddr = %p",
               current, current->next, current->is_free, current->size, current->saddr);
    current = current->next;
  }
  small_block_priv = sstart_addr_priv;
}
