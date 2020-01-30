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

#define LUCUR_VKCOMP_API
#include <lucom.h>

vkcomp *wlu_init_vk() {
  vkcomp *app = wlu_alloc(WLU_SMALL_BLOCK_PRIV, sizeof(vkcomp), NEG_ONE);
  if (!app) { PERR(WLU_ALLOC_FAILED, 0, NULL); return app; };

  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;

  return app;
}

void wlu_freeup_sc(void *data) {
  vkcomp *app = (vkcomp *) data;

  /* destory all uniform buffers */
  if (app->buffs_data) {
    for (uint32_t i = 0; i < app->bdc; i++) {
      if (app->buffs_data[i].name == 'u') {
        vkDestroyBuffer(app->device, app->buffs_data[i].buff, NULL);
        vkFreeMemory(app->device, app->buffs_data[i].mem, NULL);
        app->buffs_data[i].buff = VK_NULL_HANDLE;
        app->buffs_data[i].mem = VK_NULL_HANDLE;
      }
    }
  }
  if (app->desc_data) {
    for (uint32_t i = 0; i < app->ddc; i++) {
      if (app->desc_data[i].desc_pool) {
        vkDestroyDescriptorPool(app->device, app->desc_data[i].desc_pool, NULL);
        app->desc_data[i].desc_pool = VK_NULL_HANDLE;
      }
    }
  }
  if (app->cmd_data) {
    for (uint32_t i = 0; i < app->cdc; i++)
      if (app->cmd_data[i].cmd_buffs)
        vkFreeCommandBuffers(app->device, app->cmd_data[i].cmd_pool, app->sc_data[i].sic, app->cmd_data[i].cmd_buffs);
  }
  if (app->pipeline_cache)  /* leave like this for now */
    vkDestroyPipelineCache(app->device, app->pipeline_cache, NULL);
  if (app->gp_data) { /* leave like this for now */
    for (uint32_t i = 0; i < app->gdc; i++) {
      if (app->gp_data[i].pipeline_layout)
        vkDestroyPipelineLayout(app->device, app->gp_data[i].pipeline_layout, NULL);
      if (app->gp_data[i].render_pass)
        vkDestroyRenderPass(app->device, app->gp_data[i].render_pass, NULL);
      for (uint32_t j = 0; j < app->gp_data[i].gpc; j++)
        vkDestroyPipeline(app->device, app->gp_data[i].graphics_pipelines[j], NULL);
    }
  }

  if (app->sc_data) {
    for (uint32_t i = 0; i < app->sdc; i++) {
      if (app->sc_data[i].sc_buffs) {
        for (uint32_t j = 0; j < app->sc_data[i].sic; j++) {
          vkDestroyFramebuffer(app->device, app->sc_data[i].sc_buffs[j].fb, NULL);
          vkDestroyImageView(app->device, app->sc_data[i].sc_buffs[j].view, NULL);
          app->sc_data[i].sc_buffs[j].image = VK_NULL_HANDLE;
          app->sc_data[i].sc_buffs[j].view = VK_NULL_HANDLE;
          app->sc_data[i].sc_buffs[j].fb = VK_NULL_HANDLE;
        }
      }
      if (app->sc_data[i].swap_chain)
        vkDestroySwapchainKHR(app->device, app->sc_data[i].swap_chain, NULL);
    }
  }
}

void wlu_freeup_vk(void *data) {
  vkcomp *app = (vkcomp *) data;

  if (app->debug_report_callback)
    app->dbg_destroy_report_callback(app->instance, app->debug_report_callback, NULL);
  if (app->cmd_data) {
    for (uint32_t i = 0; i < app->cdc; i++) {
      if (app->cmd_data[i].cmd_pool)
        vkDestroyCommandPool(app->device, app->cmd_data[i].cmd_pool, NULL);
    }
  }
  if (app->pipeline_cache)  /* leave like this for now */
    vkDestroyPipelineCache(app->device, app->pipeline_cache, NULL);
  if (app->gp_data) { /* leave like this for now */
    for (uint32_t i = 0; i < app->gdc; i++) {
      if (app->gp_data[i].pipeline_layout)
        vkDestroyPipelineLayout(app->device, app->gp_data[i].pipeline_layout, NULL);
      if (app->gp_data[i].render_pass)
        vkDestroyRenderPass(app->device, app->gp_data[i].render_pass, NULL);
      for (uint32_t j = 0; j < app->gp_data[i].gpc; j++)
        vkDestroyPipeline(app->device, app->gp_data[i].graphics_pipelines[j], NULL);
    }
  }
  if (app->desc_data) {
    for (uint32_t i = 0; i < app->ddc; i++) {
      if (app->desc_data[i].desc_layouts) {
        for (uint32_t j = 0; j < app->desc_data[i].dlsc; j++) {
          if (app->desc_data[i].desc_layouts[j])
            vkDestroyDescriptorSetLayout(app->device, app->desc_data[i].desc_layouts[j], NULL);
        }
      }
      if (app->desc_data[i].desc_pool)
        vkDestroyDescriptorPool(app->device, app->desc_data[i].desc_pool, NULL);
    }
  }
  if (app->buffs_data) {
    for (uint32_t i = 0; i < app->bdc; i++) {
      if (app->buffs_data[i].buff)
        vkDestroyBuffer(app->device, app->buffs_data[i].buff, NULL);
      if (app->buffs_data[i].mem)
        vkFreeMemory(app->device, app->buffs_data[i].mem, NULL);
    }
  }
  if (app->sc_data) { /* Annihilate All Swap Chain Objects */
    for (uint32_t i = 0; i < app->sdc; i++) {
      if (app->sc_data[i].depth.view)
        vkDestroyImageView(app->device, app->sc_data[i].depth.view, NULL);
      if (app->sc_data[i].depth.image)
        vkDestroyImage(app->device, app->sc_data[i].depth.image, NULL);
      if (app->sc_data[i].depth.mem)
        vkFreeMemory(app->device, app->sc_data[i].depth.mem, NULL);
      if (app->sc_data[i].sc_buffs && app->sc_data[i].sems) {
        for (uint32_t j = 0; j < app->sc_data[i].sic; j++) {
          if (app->sc_data[i].sems[j].image && app->sc_data[i].sems[j].render) {
            vkDestroySemaphore(app->device, app->sc_data[i].sems[j].image, NULL);
            vkDestroySemaphore(app->device, app->sc_data[i].sems[j].render, NULL);
          }
          vkDestroyFramebuffer(app->device, app->sc_data[i].sc_buffs[j].fb, NULL);
          vkDestroyImageView(app->device, app->sc_data[i].sc_buffs[j].view, NULL);
        }
      }
      if (app->sc_data[i].swap_chain)
        vkDestroySwapchainKHR(app->device, app->sc_data[i].swap_chain, NULL);
    }
  }
  if (app->device) {
    vkDeviceWaitIdle(app->device);
    vkDestroyDevice(app->device, NULL);
  }
  if (app->surface)
    vkDestroySurfaceKHR(app->instance, app->surface, NULL);
  if (app->instance)
    vkDestroyInstance(app->instance, NULL);
}

VkResult wlu_otba(vkcomp *app, uint32_t index, uint32_t arr_size, wlu_data_type type) {
  switch (type) {
    case WLU_SC_DATA:
      app->sc_data = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _sc_data), NEG_ONE);
      if (!app->sc_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->sdc = arr_size; break;
    case WLU_GP_DATA:
      app->gp_data = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _gp_data), NEG_ONE);
      if (!app->gp_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->gdc = arr_size; break;
    case WLU_CMD_DATA:
      app->cmd_data = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _cmd_data), NEG_ONE);
      if (!app->cmd_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->cdc = arr_size; break;
    case WLU_BUFFS_DATA:
      app->buffs_data = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _buffs_data), NEG_ONE);
      if (!app->buffs_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->bdc = arr_size; break;
    case WLU_DESC_DATA:
      app->desc_data = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _desc_data), NEG_ONE);
      if (!app->desc_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->ddc = arr_size; break;
    case WLU_SC_DATA_MEMS:
      /**
      * Don't want to stick to minimum becuase one would have to wait on the
      * drive to complete internal operations before one can acquire another
      * images to render to. So it's recommended to add one to minImageCount
      */
      arr_size += 1;

      /* Allocate SwapChain Buffers (VkImage, VkImageView, VkFramebuffer) */
      app->sc_data[index].sc_buffs = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _swap_chain_buffers), NEG_ONE);
      if (!app->sc_data[index].sc_buffs) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }

      /* Allocate CommandBuffers, This is okay */
      app->cmd_data[index].cmd_buffs = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkCommandBuffer), NEG_ONE);
      if (!app->cmd_data[index].cmd_buffs) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }

      /* Allocate Semaphores */
      app->sc_data[index].sems = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(struct _semaphores), NEG_ONE);
      if (!app->sc_data[index].sems) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->sc_data[index].sic = arr_size; break;
    case WLU_DESC_DATA_MEMS:
      app->desc_data[index].desc_layouts = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkDescriptorSetLayout), NEG_ONE);
      if (!app->desc_data[index].desc_layouts) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }

      app->desc_data[index].desc_set = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkDescriptorSet), NEG_ONE);
      if (!app->desc_data[index].desc_set) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->desc_data[index].dlsc = arr_size; break;
    case WLU_GP_DATA_MEMS:
      app->gp_data[index].graphics_pipelines = wlu_alloc(WLU_SMALL_BLOCK_PRIV, arr_size * sizeof(VkPipeline), NEG_ONE);
      if (!app->gp_data[index].graphics_pipelines) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }
      app->gp_data[index].gpc = arr_size; break;
    default: break;
  }

  return VK_SUCCESS;
}
