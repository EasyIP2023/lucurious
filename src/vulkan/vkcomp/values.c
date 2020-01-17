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
#include <wlu/vlucur/vkall.h>
#include <vlucur/values.h>

void set_vkcomp_init_values(vkcomp *app) {
  app->dbg_destroy_report_callback = VK_NULL_HANDLE;
  app->debug_report_callback = VK_NULL_HANDLE;
  app->instance = VK_NULL_HANDLE;
  app->surface = VK_NULL_HANDLE;
  app->physical_device = VK_NULL_HANDLE;
  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;
  app->device = VK_FALSE;
  app->graphics_queue = VK_NULL_HANDLE;
  app->present_queue = VK_NULL_HANDLE;
  app->sdc = VK_NULL_HANDLE;
  app->sc_data = VK_NULL_HANDLE;
  app->gdc = VK_NULL_HANDLE;
  app->gp_data = VK_NULL_HANDLE;
  app->cdc = VK_NULL_HANDLE;
  app->cmd_data = VK_NULL_HANDLE;
  app->bdc = VK_NULL_HANDLE;
  app->buffs_data = VK_NULL_HANDLE;
  app->ddc = VK_NULL_HANDLE;
  app->desc_data = VK_NULL_HANDLE;
}

void set_sc_buffs_init_values(vkcomp *app, uint32_t cur_scd) {
  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    app->sc_data[cur_scd].sc_buffs[i].image = VK_NULL_HANDLE;
    app->sc_data[cur_scd].sc_buffs[i].view = VK_NULL_HANDLE;
  }
}

void set_sc_sems_init_values(vkcomp *app, uint32_t cur_scd) {
  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    app->sc_data[cur_scd].sems[i].image = VK_NULL_HANDLE;
    app->sc_data[cur_scd].sems[i].render = VK_NULL_HANDLE;
  }
}

void set_sc_data_init_values(vkcomp *app) {
  for (uint32_t i = 0; i < app->sdc; i++) {
    app->sc_data[i].sic = VK_NULL_HANDLE;
    app->sc_data[i].swap_chain = VK_NULL_HANDLE;
    app->sc_data[i].sc_buffs = VK_NULL_HANDLE;
    app->sc_data[i].sems = VK_NULL_HANDLE;
    app->sc_data[i].frame_buffs = VK_NULL_HANDLE;
    app->sc_data[i].depth.format = VK_NULL_HANDLE;
    app->sc_data[i].depth.view = VK_NULL_HANDLE;
    app->sc_data[i].depth.image = VK_NULL_HANDLE;
    app->sc_data[i].depth.mem = VK_NULL_HANDLE;
  }
}

void set_buffs_data_init_values(vkcomp *app) {
  for (uint32_t i = 0; i < app->bdc; i++) {
    app->buffs_data[i].buff = VK_NULL_HANDLE;
    app->buffs_data[i].mem = VK_NULL_HANDLE;
    app->buffs_data[i].name = VK_NULL_HANDLE;
  }
}

void set_cmd_data_init_values(vkcomp *app) {
  for (uint32_t i = 0; i < app->cdc; i++) {
    app->cmd_data[i].cmd_pool = VK_NULL_HANDLE;
    app->cmd_data[i].cmd_buffs = VK_NULL_HANDLE;
  }
}

void set_gp_data_init_values(vkcomp *app) {
  for (uint32_t i = 0; i < app->gdc; i++) {
    app->gp_data[i].render_pass = VK_NULL_HANDLE;
    app->gp_data[i].pipeline_layout = VK_NULL_HANDLE;
    app->gp_data[i].gpc = VK_NULL_HANDLE;
    app->gp_data[i].graphics_pipelines = VK_NULL_HANDLE;
  }
}

void set_desc_data_init_values(vkcomp *app) {
  for (uint32_t i = 0; i < app->ddc; i++) {
    app->desc_data[i].dlsc = VK_NULL_HANDLE;
    app->desc_data[i].desc_layouts = VK_NULL_HANDLE;
    app->desc_data[i].desc_pool = VK_NULL_HANDLE;
    app->desc_data[i].desc_set = VK_NULL_HANDLE;
  }
}
