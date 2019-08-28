/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>

static void set_values(vkcomp *app) {
  app->dbg_create_report_callback = VK_NULL_HANDLE;
  app->dbg_destroy_report_callback = VK_NULL_HANDLE;
  app->debug_messenger = VK_NULL_HANDLE;
  app->dbg_size = VK_NULL_HANDLE;
  app->debug_report_callbacks = VK_NULL_HANDLE;
  app->instance = VK_NULL_HANDLE;
  app->surface = VK_NULL_HANDLE;
  app->vk_layer_props = NULL;
  app->vk_layer_count = VK_NULL_HANDLE;
  app->ep_instance_props = NULL;
  app->ep_instance_count = VK_NULL_HANDLE;
  app->ep_device_props = NULL;
  app->ep_device_count = VK_NULL_HANDLE;
  // app->device_properties;
  // app->device_features;
  // app->memory_properties;
  app->physical_device = VK_NULL_HANDLE;
  app->queue_create_infos = NULL;
  app->queue_families = NULL;
  app->queue_family_count = VK_NULL_HANDLE;
  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;
  app->device = VK_FALSE;
  app->graphics_queue = VK_NULL_HANDLE;
  app->present_queue = VK_NULL_HANDLE;
  app->sc_buffs = VK_NULL_HANDLE;
  app->swap_chain = VK_NULL_HANDLE;
  app->sc_img_count = VK_NULL_HANDLE;
  app->render_pass = VK_NULL_HANDLE;
  app->pipeline_cache = VK_NULL_HANDLE;
  app->pipeline_layout = VK_NULL_HANDLE;
  app->graphics_pipeline = VK_NULL_HANDLE;
  app->render_pass = VK_NULL_HANDLE;
  app->sc_frame_buffs = VK_NULL_HANDLE;
  app->cmd_pool = VK_NULL_HANDLE;
  app->cmd_buffs = VK_NULL_HANDLE;
  app->img_semaphore = VK_NULL_HANDLE;
  app->render_semaphore = VK_NULL_HANDLE;
  app->draw_fence = VK_NULL_HANDLE;
  app->depth.view = VK_NULL_HANDLE;
  app->depth.image = VK_NULL_HANDLE;
  app->depth.mem = VK_NULL_HANDLE;
  app->desc_count = VK_NULL_HANDLE;
  app->desc_layout = VK_NULL_HANDLE;
  app->desc_pool = VK_NULL_HANDLE;
  app->desc_set = VK_NULL_HANDLE;
}

vkcomp *wlu_init_vk() {
  vkcomp *app = calloc(sizeof(vkcomp), sizeof(vkcomp));
  if (!app) return NULL;
  set_values(app);
  return app;
}

void wlu_freeup_vk(void *data) {
  vkcomp *app = (vkcomp *) data;

  if (app->debug_report_callbacks) {
    for (uint32_t i = 0; i < app->dbg_size; i++) {
      app->dbg_destroy_report_callback(app->instance, app->debug_report_callbacks[i], NULL);
      app->debug_report_callbacks[i] = NULL;
    }
    free(app->debug_report_callbacks);
  }
  if (app->vk_layer_props)
    free(app->vk_layer_props);
  if (app->ep_instance_props)
    free(app->ep_instance_props);
  if (app->ep_device_props)
    free(app->ep_device_props);
  if (app->queue_families)
    free(app->queue_families);
  if (app->queue_create_infos)
    free(app->queue_create_infos);
  if (app->depth.view)
    vkDestroyImageView(app->device, app->depth.view, NULL);
  if (app->depth.image)
    vkDestroyImage(app->device, app->depth.image, NULL);
  if (app->depth.mem)
    vkFreeMemory(app->device, app->depth.mem, NULL);
  if (app->render_semaphore)
    vkDestroySemaphore(app->device, app->render_semaphore, NULL);
  if (app->img_semaphore)
    vkDestroySemaphore(app->device, app->img_semaphore, NULL);
  if (app->draw_fence)
    vkDestroyFence(app->device, app->draw_fence, NULL);
  if (app->cmd_buffs)
    vkFreeCommandBuffers(app->device, app->cmd_pool, app->sc_img_count, app->cmd_buffs);
  if (app->cmd_pool)
    vkDestroyCommandPool(app->device, app->cmd_pool, NULL);
  if (app->sc_frame_buffs) {
    for (uint32_t i = 0; i < app->sc_img_count; i++) {
      vkDestroyFramebuffer(app->device, app->sc_frame_buffs[i], NULL);
      app->sc_frame_buffs[i] = VK_NULL_HANDLE;
    }
    free(app->sc_frame_buffs);
  }
  if (app->pipeline_cache)
    vkDestroyPipelineCache(app->device, app->pipeline_cache, NULL);
  if (app->pipeline_layout)
    vkDestroyPipelineLayout(app->device, app->pipeline_layout, NULL);
  if (app->graphics_pipeline)
    vkDestroyPipeline(app->device, app->graphics_pipeline, NULL);
  if (app->desc_layout) {
    for (uint32_t i = 0; i < app->desc_count; i++)
      vkDestroyDescriptorSetLayout(app->device, app->desc_layout[i], NULL);
    free(app->desc_layout);
  }
  if (app->desc_set)
    free(app->desc_set);
  if (app->desc_pool)
    vkDestroyDescriptorPool(app->device, app->desc_pool, NULL);
  if (app->vertex_data.buff)
    vkDestroyBuffer(app->device, app->vertex_data.buff, NULL);
  if (app->vertex_data.mem)
    vkFreeMemory(app->device, app->vertex_data.mem, NULL);
  if (app->uniform_data.buff)
    vkDestroyBuffer(app->device, app->uniform_data.buff, NULL);
  if (app->uniform_data.mem)
    vkFreeMemory(app->device, app->uniform_data.mem, NULL);
  if (app->render_pass)
    vkDestroyRenderPass(app->device, app->render_pass, NULL);
  if (app->sc_buffs) {
    for (uint32_t i = 0; i < app->sc_img_count; i++) {
      vkDestroyImageView(app->device, app->sc_buffs[i].view, NULL);
      app->sc_buffs[i].view = VK_NULL_HANDLE;
    }
    free(app->sc_buffs);
  }
  if (app->swap_chain)
    vkDestroySwapchainKHR(app->device, app->swap_chain, NULL);
  if (app->device) {
    vkDeviceWaitIdle(app->device);
    vkDestroyDevice(app->device, NULL);
  }
  if (app->surface)
    vkDestroySurfaceKHR(app->instance, app->surface, NULL);
  if (app->instance)
    vkDestroyInstance(app->instance, NULL);

  set_values(app);
  if (app)
    free(app);
  app = NULL;
}
