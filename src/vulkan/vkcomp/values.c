/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 EasyIP2023
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
#include <vlucur/values.h>

void set_vkcomp_init_values(vkcomp *app) {
  app->dbg_create_report_callback = VK_NULL_HANDLE;
  app->dbg_destroy_report_callback = VK_NULL_HANDLE;
  app->debug_messenger = VK_NULL_HANDLE;
  app->debug_report_callbacks = VK_NULL_HANDLE;
  app->dbg_size = VK_NULL_HANDLE;
  app->instance = VK_NULL_HANDLE;
  app->surface = VK_NULL_HANDLE;
  app->vk_layer_props = NULL;
  app->vlc = VK_NULL_HANDLE;
  app->ep_instance_props = NULL;
  app->eic = VK_NULL_HANDLE;
  app->ep_device_props = NULL;
  app->edc = VK_NULL_HANDLE;
  app->physical_device = VK_NULL_HANDLE;
  app->queue_create_infos = NULL;
  app->queue_families = NULL;
  app->queue_family_count = VK_NULL_HANDLE;
  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;
  app->device = VK_FALSE;
  app->graphics_queue = VK_NULL_HANDLE;
  app->present_queue = VK_NULL_HANDLE;
  app->scc = VK_NULL_HANDLE;
  app->sc = VK_NULL_HANDLE;
  app->render_pass = VK_NULL_HANDLE;
  app->pipeline_cache = VK_NULL_HANDLE;
  app->pipeline_layout = VK_NULL_HANDLE;
  app->graphics_pipeline = VK_NULL_HANDLE;
  app->cpc = VK_NULL_HANDLE;
  app->cmd_pbs = VK_NULL_HANDLE;
  // app->ubd = VK_NULL_HANDLE;
  app->bdc = VK_NULL_HANDLE;
  app->buffs_data = VK_NULL_HANDLE;
  app->desc_count = VK_NULL_HANDLE;
  app->desc_layouts = VK_NULL_HANDLE;
  app->desc_pool = VK_NULL_HANDLE;
  app->desc_set = VK_NULL_HANDLE;
}

void set_sc_init_values(vkcomp *app) {
  for (uint32_t i = app->scc; i < (app->scc+1); i++) {
    app->sc[i].swap_chain = VK_NULL_HANDLE;
    app->sc[i].sc_buffs = VK_NULL_HANDLE;
    app->sc[i].sems = VK_NULL_HANDLE;
    app->sc[i].frame_buffs = VK_NULL_HANDLE;
    app->sc[i].depth.view = VK_NULL_HANDLE;
    app->sc[i].depth.image = VK_NULL_HANDLE;
    app->sc[i].depth.mem = VK_NULL_HANDLE;
  }
}
