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

/* To access header struct information in C source file */
#define LUCUR_WAYLAND_API
#define LUCUR_WAYLAND_CLIENT_API
#define LUCUR_VKCOMP_API
#include <lucom.h>

/**
* Struct that stores block metadata
* Using linked list to keep track of memory allocated
* next     | points to next memory block
* is_free  | checks if memory block is free or not
* size     | allocated memory size
* abytes   | available bytes left in block
* addr     | Currenet address of the block
* saddr    | Starting address for the block where data is assigned
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
#define ALLOC_ERR (void*)-1
#define ERR64 -1

/**
* Globals used to keep track of memory blocks
* sstart_addr: Keep track of first allocated small block address
* large_block: A struct to keep track of one large allocated block
* small_block_list: linked list for smaller allocated blocks
*/
static void *sstart_addr = NULL;
static wlu_mem_block_t *large_block = NULL;
static wlu_mem_block_t *small_block_list = NULL;

/**
* Helps in ensuring one does not waste cycles in context switching
* First check if sub-block was allocated and is currently free
* If block not free, sub allocate more from larger memory block
*/
static wlu_mem_block_t *get_free_block(size_t bytes) {
	wlu_mem_block_t *current = sstart_addr;
	while (current->next) {
    if (current->is_free && current->size >= bytes) {
      current->is_free = false;
      return current;
    }
    current = current->next;
	}

  if (large_block->abytes >= bytes) {
    wlu_mem_block_t *block = current->addr;

    block->next = NULL;
    block->is_free = false;
    block->size = bytes;

    /* Decrement large block available memory */
    large_block->abytes -= (BLOCK_SIZE + bytes);

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

    return block;
  }

  return NULL;
}

static wlu_mem_block_t *alloc_mem_block(size_t bytes) {
  wlu_mem_block_t *block = NULL;

  /* Allows for zeros to be written into values of bytes allocated */
  int fd = open("/dev/zero", O_RDWR);
  if (fd == ERR64) {
    wlu_log_me(WLU_DANGER, "[x] open: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);
  if (block == ALLOC_ERR) {
    wlu_log_me(WLU_DANGER, "[x] mmap: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  block->next = NULL;
  block->is_free = false;
  block->size = block->abytes = bytes;

  /* Put saddr at an address that doesn't contain metadata */
  block->addr = block;
  block->saddr = BLOCK_SIZE + block;
finish_alloc_mem_block:
  if (close(fd) == ERR64)
    wlu_log_me(WLU_DANGER, "[x] close: %s", strerror(errno));
  return block;
}

void *wlu_alloc(wlu_block_type type, size_t bytes) {
  wlu_mem_block_t *nblock = NULL;

  /**
  * This will create large block of memory
  * Then create a linked list of smaller blocks from the larger one
  * Retrieve last used memory block
  * O(1) appending to end of linked-list
  * reset linked list to starting address of linked list
  */
  switch (type) {
    case WLU_LARGE_BLOCK:
      /* If large block allocated don't allocate another one */
      if (large_block) return NULL;

      nblock = alloc_mem_block(bytes);
      if (!nblock) return NULL;
      large_block = nblock;

      /**
      * Set small block allocation addr to address that
      * doesn't include larger block metadata
      */
      small_block_list = sstart_addr = nblock->saddr;
      small_block_list->addr = small_block_list;
      break;
    case WLU_SMALL_BLOCK:
      /* If large block not allocated return NULL until allocated */
      if (!large_block) return NULL;

      nblock = get_free_block(bytes);
      if (!nblock) return NULL;

      /* set small block list at current addr of the last block in the list */
      small_block_list = nblock->prv_addr;
      small_block_list->next = nblock;

      /* Move back to previous block (for return status) */
      nblock = small_block_list;
      small_block_list = sstart_addr; /* RESET */
      break;
    default: break;
  }

  return nblock->saddr;
}

bool wlu_otma(wlu_otma_mems ma) {
  size_t size = 0;

  size += (ma.inta_cnt * sizeof(int));
  size += (ma.cha_cnt * sizeof(char)); /* sizeof(char) is for formality */
  size += (ma.fla_cnt * sizeof(float));
  size += (ma.dba_cnt * sizeof(double));

  size += (ma.wclient_cnt * sizeof(wclient));

  size += (ma.vkcomp_cnt * sizeof(vkcomp));
  size += (ma.vkext_props_cnt * sizeof(VkExtensionProperties));
  size += (ma.vkval_layer_cnt * sizeof(VkLayerProperties));
  size += (ma.gp_cnt * sizeof(VkPipeline));
  size += (ma.desc_cnt * sizeof(VkDescriptorSet));
  size += (ma.desc_cnt * sizeof(VkDescriptorSetLayout));
  size += (ma.si_cnt * sizeof(struct swap_chain_buffers));
  size += (ma.si_cnt * sizeof(VkFramebuffer));
  size += (ma.si_cnt * sizeof(VkCommandBuffer));
  size += (ma.si_cnt * sizeof(struct semaphores));
  size += (ma.scd_cnt * sizeof(struct _sc_data));
  size += (ma.gpd_cnt * sizeof(struct _gp_data));
  size += (ma.cmdd_cnt * sizeof(struct _cmd_data));
  size += (ma.bd_cnt * sizeof(struct _buffs_data));
  size += (ma.dd_cnt * sizeof(struct _desc_data));

  if (!wlu_alloc(WLU_LARGE_BLOCK, size)) return false;

  return true;
}

/**
* Freeing memory in this case means to override the contents
* If memory address can't be accessed break
* Node is most likely the end of linked list
*/
void wlu_free_block(void *addr) {
  wlu_mem_block_t *current = small_block_list;
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
void wlu_release_block() {
  if (!large_block) return;
  if (munmap(large_block, BLOCK_SIZE + large_block->size) == ERR64) {
    wlu_log_me(WLU_DANGER, "[x] munmap: %s", strerror(errno));
    return;
  }
  large_block = NULL;
}

void wlu_print_mb() {
  wlu_mem_block_t *current = small_block_list;
  while(current->next) {
    wlu_log_me(WLU_INFO, "current block = %p, next block = %p, isfree = %d, block size = %d, saddr = %p",
               current, current->next, current->is_free, current->size, current->saddr);
    current = current->next;
  }
  small_block_list = sstart_addr;
}
