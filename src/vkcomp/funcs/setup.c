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

#define LUCUR_VKCOMP_API
#include <lucom.h>

vkcomp *dlu_init_vk() {
  vkcomp *app = dlu_alloc(DLU_SMALL_BLOCK_PRIV, sizeof(vkcomp));
  if (!app) { PERR(DLU_ALLOC_FAILED, 0, NULL); return app; };

  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;

  return app;
}

void dlu_freeup_sc(void *data) {
  vkcomp *app = (vkcomp *) data;

  /* destory all uniform buffers */
  if (app->buff_data) {
    for (uint32_t i = 0; i < app->bdc; i++) {
      if (app->buff_data[i].name == 'u') {
        if (app->buff_data[i].buff) {
          vkDestroyBuffer(app->device, app->buff_data[i].buff, NULL);
          app->buff_data[i].buff = VK_NULL_HANDLE;
        }
        if (app->buff_data[i].mem) {
          vkFreeMemory(app->device, app->buff_data[i].mem, NULL);
          app->buff_data[i].mem = VK_NULL_HANDLE;
        }
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

  if (app->pipeline_cache) {
    vkDestroyPipelineCache(app->device, app->pipeline_cache, NULL);
    app->pipeline_cache = VK_NULL_HANDLE;
  }

  if (app->gp_data) {
    for (uint32_t i = 0; i < app->gdc; i++) {
      if (app->gp_data[i].pipeline_layout) {
        vkDestroyPipelineLayout(app->device, app->gp_data[i].pipeline_layout, NULL);
        app->gp_data[i].pipeline_layout = VK_NULL_HANDLE;
      }
      if (app->gp_data[i].render_pass) {
        vkDestroyRenderPass(app->device, app->gp_data[i].render_pass, NULL);
        app->gp_data[i].render_pass = VK_NULL_HANDLE;
      }
      for (uint32_t j = 0; j < app->gp_data[i].gpc; j++) {
        if (app->gp_data[i].graphics_pipelines[j]) {
          vkDestroyPipeline(app->device, app->gp_data[i].graphics_pipelines[j], NULL);
          app->gp_data[i].graphics_pipelines[j] = VK_NULL_HANDLE;
        }
      }
    }
  }

  if (app->sc_data) {
    for (uint32_t i = 0; i < app->sdc; i++) {
      if (app->sc_data[i].sc_buffs) {
        for (uint32_t j = 0; j < app->sc_data[i].sic; j++) {
          if (app->sc_data[i].sc_buffs[j].fb) {
            vkDestroyFramebuffer(app->device, app->sc_data[i].sc_buffs[j].fb, NULL);
            app->sc_data[i].sc_buffs[j].fb = VK_NULL_HANDLE;
          }
          if (app->sc_data[i].sc_buffs[j].view) {
            vkDestroyImageView(app->device, app->sc_data[i].sc_buffs[j].view, NULL);
            app->sc_data[i].sc_buffs[j].view = VK_NULL_HANDLE;
          }
          if (app->sc_data[i].sc_buffs[j].image) {
            vkDestroyImage(app->device, app->sc_data[i].sc_buffs[j].image, NULL);
            app->sc_data[i].sc_buffs[j].image = VK_NULL_HANDLE;
          }
        }
      }

      if (app->sc_data[i].swap_chain) {
        vkDestroySwapchainKHR(app->device, app->sc_data[i].swap_chain, NULL);
        app->sc_data[i].swap_chain = VK_NULL_HANDLE;
      }
    }
  }
}

void dlu_freeup_vk(void *data) {
  vkcomp *app = (vkcomp *) data;

  /* Synchronous wait for present queue to empty. So that all objects can be properly destroyed */
  if (app->present_queue)
    vkQueueWaitIdle(app->present_queue);

  if (app->debug_report_callback)
    app->dbg_destroy_report_callback(app->instance, app->debug_report_callback, NULL);

  if (app->cmd_data) {
    for (uint32_t i = 0; i < app->cdc; i++) {
      if (app->cmd_data[i].cmd_pool)
        vkDestroyCommandPool(app->device, app->cmd_data[i].cmd_pool, NULL);
    }
  }

  if (app->pipeline_cache)
    vkDestroyPipelineCache(app->device, app->pipeline_cache, NULL);
 
  if (app->text_data) {
    for (uint32_t i = 0; i < app->tdc; i++) {
      if (app->text_data[i].sampler)
        vkDestroySampler(app->device, app->text_data[i].sampler, NULL);
      if (app->text_data[i].view)
        vkDestroyImageView(app->device, app->text_data[i].view, NULL);
      if (app->text_data[i].image)
        vkDestroyImage(app->device, app->text_data[i].image, NULL);
      if (app->text_data[i].mem)
        vkFreeMemory(app->device, app->text_data[i].mem, NULL);
    }
  }

  if (app->gp_data) {
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
      if (app->desc_data[i].layouts) {
        for (uint32_t j = 0; j < app->desc_data[i].dlsc; j++) {
          if (app->desc_data[i].layouts[j])
            vkDestroyDescriptorSetLayout(app->device, app->desc_data[i].layouts[j], NULL);
        }
      }
      if (app->desc_data[i].desc_pool)
        vkDestroyDescriptorPool(app->device, app->desc_data[i].desc_pool, NULL);
    }
  }

  if (app->buff_data) {
    for (uint32_t i = 0; i < app->bdc; i++) {
      if (app->buff_data[i].buff)
        vkDestroyBuffer(app->device, app->buff_data[i].buff, NULL);
      if (app->buff_data[i].mem)
        vkFreeMemory(app->device, app->buff_data[i].mem, NULL);
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
      if (app->sc_data[i].sc_buffs && app->sc_data[i].syncs) {
        for (uint32_t j = 0; j < app->sc_data[i].sic; j++) {
          if (app->sc_data[i].syncs[j].sem.image)
            vkDestroySemaphore(app->device, app->sc_data[i].syncs[j].sem.image, NULL);
          if (app->sc_data[i].syncs[j].sem.render)
            vkDestroySemaphore(app->device, app->sc_data[i].syncs[j].sem.render, NULL);
          if (app->sc_data[i].syncs[j].fence.render)
            vkDestroyFence(app->device, app->sc_data[i].syncs[j].fence.render, NULL);
          if (app->sc_data[i].sc_buffs[j].fb)
            vkDestroyFramebuffer(app->device, app->sc_data[i].sc_buffs[j].fb, NULL);
          if (app->sc_data[i].sc_buffs[j].view)
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
