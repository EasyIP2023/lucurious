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

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lucom.h>
#include "../../include/vkcomp/types.h"
#include "../../include/drm/types.h"

/**
* Struct that stores block metadata
* Using linked list to keep track of memory allocated
* next     | points to next memory block
* size     | allocated memory size
* abytes   | available bytes left in block
* addr     | Current address of the block
* saddr    | Starting address of the block where data is assigned
* prv_addr | Address of the previous block
*/
typedef struct mblock {
  struct mblock *next;
  size_t size;
  size_t abytes;
  void *addr;
  void *saddr;
  void *prv_addr;
} dlu_mem_block_t;

#define BLOCK_SIZE sizeof(dlu_mem_block_t)

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
static dlu_mem_block_t *large_block_priv = NULL;
static dlu_mem_block_t *small_block_priv = NULL;

static void *sstart_addr_shared = NULL;
static dlu_mem_block_t *large_block_shared = NULL;
static dlu_mem_block_t *small_block_shared = NULL;

/**
* Helps in ensuring one does not waste cycles in context switching
* First check if sub-block was allocated and is currently free
* If block not free, sub allocate more from larger memory block
*/
static dlu_mem_block_t *get_free_block(dlu_block_type type, size_t bytes) {
  dlu_mem_block_t *current = NULL;
  size_t abytes = 0;

  /**
  * This allows for O(1) allocation
  * If next block doesn't exists use large_block_priv/shared->saddr address
  * else set current to next block (which would be the block waiting to be allocated)
  */
  switch(type) {
    case DLU_SMALL_BLOCK_PRIV:
      abytes = large_block_priv->abytes;
      current = (!small_block_priv->next) ? sstart_addr_priv : small_block_priv->next;
      break;
    case DLU_SMALL_BLOCK_SHARED:
      abytes = large_block_shared->abytes;
      current = (!small_block_shared->next) ? sstart_addr_shared : small_block_shared->next;
      break;
    default: break;
  }

  if (!current) return NULL;

  if (abytes >= bytes) {
    /* current block thats about to be allocated set few metadata */
    dlu_mem_block_t *block = current->addr;
    block->size = bytes;
    /* Put saddr at an address that doesn't contain metadata */
    block->saddr = BLOCK_SIZE + current->addr;

    /**
    * Set next blocks metadata
    * This is written this way because one needs the return address
    * to be the starting address of the next block not the current one.
    * Basically offset the memory address. Thus, allocating space.
    */
    block = current->addr + BLOCK_SIZE + bytes;
    block->prv_addr = current->addr;

    block->addr = block;
    block->next = NULL;
    block->abytes = 0;
    block->saddr = NULL;

    /* Decrement larger block available memory */
    switch(type) {
      case DLU_SMALL_BLOCK_SHARED: large_block_shared->abytes -= (BLOCK_SIZE + bytes); break;
      case DLU_SMALL_BLOCK_PRIV: large_block_priv->abytes -= (BLOCK_SIZE + bytes); break;
      default: break;
    }

    return block;
  }

  return NULL;
}

static dlu_mem_block_t *alloc_mem_block(dlu_block_type type, size_t bytes) {
  dlu_mem_block_t *block = NULL;

  /* Allows for zeros to be written into values of bytes allocated */
  int fd = open("/dev/zero", O_RDWR);
  if (fd == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] open: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  int flags = (type == DLU_LARGE_BLOCK_SHARED) ? MAP_SHARED : MAP_PRIVATE;
  block = mmap(NULL, BLOCK_SIZE + bytes, PROT_READ | PROT_WRITE, flags | MAP_ANONYMOUS, fd, 0);
  if (block == MAP_FAILED) {
    dlu_log_me(DLU_DANGER, "[x] mmap: %s", strerror(errno));
    goto finish_alloc_mem_block;
  }

  block->next = NULL;
  block->size = block->abytes = bytes;

  /* Put saddr at an address that doesn't contain metadata */
  block->addr = block;
  block->saddr = BLOCK_SIZE + block;
finish_alloc_mem_block:
  if (close(fd) == NEG_ONE)
    dlu_log_me(DLU_DANGER, "[x] close: %s", strerror(errno));
  return block;
}

/**
* This function is reserve for one time use. Only used when allocating space for struct members
* It works similiar to how sbrk works. Basically it creates a new block of memory, but it returns
* the ending address of the previous block.
*/
void *dlu_alloc(dlu_block_type type, size_t bytes) {
  dlu_mem_block_t *nblock = NULL;

  /**
  * This will create large block of memory
  * Then create a linked list of smaller blocks from the larger one
  * Retrieve last used memory block
  * O(1) appending to end of linked-list
  * reset linked list to starting address of linked list
  */

  /* TODO: Possible rewrite of switch statement */
  switch (type) {
    case DLU_LARGE_BLOCK_PRIV:
      /* If large block allocated don't allocate another one */
      if (large_block_priv) { PERR(DLU_ALREADY_ALLOC, 0, NULL); return NULL; }

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
    case DLU_SMALL_BLOCK_PRIV:
      /* If large block not allocated return NULL until allocated */
      if (!large_block_priv) return NULL;

      nblock = get_free_block(type, bytes);
      if (!nblock) return NULL;

      /* set small block list to address of the previous block in the list */
      small_block_priv = nblock->prv_addr;
      small_block_priv->next = nblock;

      /* Move back to previous block (for return status) */
      nblock = small_block_priv;
      break;
    case DLU_LARGE_BLOCK_SHARED:
      /* If large block allocated don't allocate another one */
      if (large_block_shared) { PERR(DLU_ALREADY_ALLOC, 0, NULL); return NULL; }

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
    case DLU_SMALL_BLOCK_SHARED:
      /* If large block not allocated return NULL until allocated */
      if (!large_block_shared) return NULL;

      nblock = get_free_block(type, bytes);
      if (!nblock) return NULL;

      /* set small block list to address of the previous block in the list */
      small_block_shared = nblock->prv_addr;
      small_block_shared->next = nblock;

      /* Move back to previous block (for return status) */
      nblock = small_block_shared;
      break;
    default: break;
  }

  return nblock->saddr;
}

bool dlu_otma(dlu_block_type type, dlu_otma_mems ma) {
  size_t size = 0;

  if (type == DLU_SMALL_BLOCK_PRIV || type == DLU_SMALL_BLOCK_SHARED) {
    PERR(DLU_OP_NOT_PERMITED, 0, NULL);
    return false;
  }

  if (large_block_priv) { PERR(DLU_ALREADY_ALLOC, 0, NULL); return false; }
  if (large_block_shared) { PERR(DLU_ALREADY_ALLOC, 0, NULL); return false; }

  size += (ma.inta_cnt * sizeof(int));
  size += (ma.cha_cnt * sizeof(char)); /* sizeof(char) is for formality */
  size += (ma.fla_cnt * sizeof(float));
  size += (ma.dba_cnt * sizeof(double));

  size += (ma.vkcomp_cnt * sizeof(vkcomp));
  size += (ma.vkext_props_cnt * sizeof(VkExtensionProperties));
  size += (ma.vkval_layer_cnt * sizeof(VkLayerProperties));

  size += (ma.si_cnt * sizeof(struct _swap_chain_buffers));
  size += (ma.si_cnt * sizeof(struct _synchronizers));
  size += (ma.scd_cnt * sizeof(struct _sc_data));

  size += (ma.gp_cnt * sizeof(VkPipeline));
  size += (ma.gpd_cnt * sizeof(struct _gp_data));

  size += (ma.si_cnt * sizeof(VkCommandBuffer));
  size += (ma.cmdd_cnt * sizeof(struct _cmd_data));

  size += (ma.bd_cnt * sizeof(struct _buff_data));

  size += (ma.desc_cnt * sizeof(VkDescriptorSet));
  size += (ma.desc_cnt * sizeof(VkDescriptorSetLayout));
  size += (ma.dd_cnt * sizeof(struct _desc_data));

  size += (ma.td_cnt * sizeof(struct _text_data));
  size += (ma.dis_cnt * sizeof(struct _dis_data));

  size += (ma.drmc_cnt * sizeof(dlu_drm_core));
  size += (ma.dod_cnt * sizeof(struct _output_data));

  if (!dlu_alloc(type, size)) return false;

  return true;
}

bool dlu_otba(dlu_data_type type, void *addr, uint32_t index, uint32_t arr_size) {
  switch (type) {
    case DLU_SC_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->sc_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _sc_data));
        if (!app->sc_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->sdc = arr_size; return true;
      }
    case DLU_GP_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->gp_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _gp_data));
        if (!app->gp_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->gdc = arr_size; return true;
      }
    case DLU_CMD_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->cmd_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _cmd_data));
        if (!app->cmd_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->cdc = arr_size; return true;
      }
    case DLU_BUFF_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->buff_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _buff_data));
        if (!app->buff_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->bdc = arr_size; return true;
      }
    case DLU_DESC_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->desc_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _desc_data));
        if (!app->desc_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->ddc = arr_size; return true;
      }
    case DLU_TEXT_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->text_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _text_data));
        if (!app->text_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->tdc = arr_size; return true;
      }
    case DLU_DIS_DATA:
      {
        vkcomp *app = (vkcomp *) addr;
        app->dis_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _dis_data));
        if (!app->dis_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->dpc = arr_size; return true;
      }
    case DLU_SC_DATA_MEMS:
      {
        vkcomp *app = (vkcomp *) addr;
        /**
        * Don't want to stick to minimum because one would have to wait on the
        * drive to complete internal operations before one can acquire another
        * images to render to. So it's recommended to add one to minImageCount
        */
        arr_size += 1;

        /* Allocate SwapChain Buffers (VkImage, VkImageView, VkFramebuffer) */
        app->sc_data[index].sc_buffs = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _swap_chain_buffers));
        if (!app->sc_data[index].sc_buffs) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }

        /* Allocate CommandBuffers, This is okay */
        app->cmd_data[index].cmd_buffs = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkCommandBuffer));
        if (!app->cmd_data[index].cmd_buffs) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }

        /* Allocate Semaphores */
        app->sc_data[index].syncs = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _synchronizers));
        if (!app->sc_data[index].syncs) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->sc_data[index].sic = arr_size; return true;
      }
    case DLU_DESC_DATA_MEMS:
      {
        vkcomp *app = (vkcomp *) addr;
        app->desc_data[index].layouts = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkDescriptorSetLayout));
        if (!app->desc_data[index].layouts) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }

        app->desc_data[index].desc_set = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkDescriptorSet));
        if (!app->desc_data[index].desc_set) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->desc_data[index].dlsc = arr_size; return true;
      }
    case DLU_GP_DATA_MEMS:
      {
        vkcomp *app = (vkcomp *) addr;
        app->gp_data[index].graphics_pipelines = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkPipeline));
        if (!app->gp_data[index].graphics_pipelines) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        app->gp_data[index].gpc = arr_size; return true;
      }
    case DLU_DEVICE_OUTPUT_DATA:
      {
        dlu_drm_core *core = (dlu_drm_core *) addr;
        core->device.output_data = dlu_alloc(DLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _output_data));
        if (!core->device.output_data) { PERR(DLU_ALLOC_FAILED, 0, NULL); return false; }
        core->device.odc = arr_size; return true;
      }
    default: break;
  }

  /* They somehow passed compiler checks and failed here */
  return false;
}

/**
* Releasing memory in this case means to
* unmap all virtual pages (remove page tables)
*/
void dlu_release_blocks() {
  if (large_block_priv) {
    if (munmap(large_block_priv, BLOCK_SIZE + large_block_priv->size) == NEG_ONE) {
      dlu_log_me(DLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    large_block_priv = NULL;
  }

  if (large_block_shared) {
    if (munmap(large_block_shared, BLOCK_SIZE + large_block_shared->size) == NEG_ONE) {
      dlu_log_me(DLU_DANGER, "[x] munmap: %s", strerror(errno));
      return;
    }
    large_block_shared = NULL;
  }
}

/* This is an INAPI_CALL */
void dlu_print_mb(dlu_block_type type) {
  dlu_mem_block_t *current = (type == DLU_SMALL_BLOCK_SHARED) ? sstart_addr_shared : sstart_addr_priv;
  while (current->next) {
    dlu_log_me(DLU_INFO, "current block = %p, next block = %p, block size = %d, saddr = %p",
                          current, current->next, current->size, current->saddr);
    current = current->next;
  }
}
