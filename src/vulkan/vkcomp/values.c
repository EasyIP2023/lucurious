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
  app->vl_props = NULL;
  app->physical_device = VK_NULL_HANDLE;
  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;
  app->device = VK_FALSE;
  app->graphics_queue = VK_NULL_HANDLE;
  app->present_queue = VK_NULL_HANDLE;
  app->scc = VK_NULL_HANDLE;
  app->sc = VK_NULL_HANDLE;
  app->gpc = VK_NULL_HANDLE;
  app->gp_data = VK_NULL_HANDLE;
  app->cpc = VK_NULL_HANDLE;
  app->cmd_pbs = VK_NULL_HANDLE;
  app->bdc = VK_NULL_HANDLE;
  app->buffs_data = VK_NULL_HANDLE;
  app->ddc = VK_NULL_HANDLE;
  app->desc_data = VK_NULL_HANDLE;
}

void set_sc_init_values(vkcomp *app) {
  app->sc[app->scc].sic = VK_NULL_HANDLE;
  app->sc[app->scc].swap_chain = VK_NULL_HANDLE;
  app->sc[app->scc].sc_buffs = VK_NULL_HANDLE;
  app->sc[app->scc].sems = VK_NULL_HANDLE;
  app->sc[app->scc].frame_buffs = VK_NULL_HANDLE;
  app->sc[app->scc].depth.format = VK_NULL_HANDLE;
  app->sc[app->scc].depth.view = VK_NULL_HANDLE;
  app->sc[app->scc].depth.image = VK_NULL_HANDLE;
  app->sc[app->scc].depth.mem = VK_NULL_HANDLE;
}

void set_sc_buffs_init_values(vkcomp *app, uint32_t cur_sc) {
  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    app->sc[cur_sc].sc_buffs[i].image = VK_NULL_HANDLE;
    app->sc[cur_sc].sc_buffs[i].view = VK_NULL_HANDLE;
  }
}

void set_sc_sems_init_values(vkcomp *app, uint32_t cur_sc) {
  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    app->sc[cur_sc].sems[i].image = VK_NULL_HANDLE;
    app->sc[cur_sc].sems[i].render = VK_NULL_HANDLE;
  }
}

void set_cmd_pbs_init_values(vkcomp *app) {
  app->cmd_pbs[app->cpc].cmd_pool = VK_NULL_HANDLE;
  app->cmd_pbs[app->cpc].cmd_buffs = VK_NULL_HANDLE;
}

void set_buffs_init_values(vkcomp *app) {
  app->buffs_data[app->bdc].buff = VK_NULL_HANDLE;
  app->buffs_data[app->bdc].mem = VK_NULL_HANDLE;
  app->buffs_data[app->bdc].buff_info.buffer = VK_NULL_HANDLE;
  app->buffs_data[app->bdc].buff_info.offset = VK_NULL_HANDLE;
  app->buffs_data[app->bdc].buff_info.range = VK_NULL_HANDLE;
  app->buffs_data[app->bdc].name = VK_NULL_HANDLE;
}

void set_desc_data_init_values(vkcomp *app) {
  app->desc_data[app->ddc].dc = VK_NULL_HANDLE;
  app->desc_data[app->ddc].desc_layouts = VK_NULL_HANDLE;
  app->desc_data[app->ddc].desc_pool = VK_NULL_HANDLE;
  app->desc_data[app->ddc].desc_set = VK_NULL_HANDLE;
}

void set_gp_data_init_values(vkcomp *app) {
  app->gp_data[app->gpc].render_pass = VK_NULL_HANDLE;
  app->gp_data[app->gpc].pipeline_layout = VK_NULL_HANDLE;
  app->gp_data[app->gpc].graphics_pipelines = VK_NULL_HANDLE;
}
