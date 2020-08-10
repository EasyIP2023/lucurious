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

#define LUCUR_DRM_API
#include <lucom.h>

/* Function name, parameters, logic may change */
bool dlu_drm_do_modeset(dlu_drm_core *core, uint32_t cur_bi) {

  if (core->buff_data[cur_bi].odid == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_drm_create_fb(3). Before mode setting is possible");
    return false;
  }

  /* Connecting a framebuffer to a Plane -> **"CRTC"** -> Encoder -> **"Connector"** pair. Perform actual modesetting */
  if (drmModeSetCrtc(core->device.kmsfd, core->output_data[core->buff_data[cur_bi].odid].crtc_id, core->buff_data[cur_bi].fb_id, 0, 0,
                     &core->output_data[core->buff_data[cur_bi].odid].conn_id, 1, &core->output_data[core->buff_data[cur_bi].odid].mode)) {
    dlu_log_me(DLU_DANGER, "[x] drmModeSetCrtc: %s", strerror(errno));
    return false;
  }

  return true;
}

bool dlu_drm_do_page_flip(dlu_drm_core *core, uint32_t cur_bi, void *user_data) {

  /* Schedules a buffer flip for the next vblank. Fully asynchronous */
  if (drmModePageFlip(core->device.kmsfd, core->output_data[core->buff_data[cur_bi].odid].crtc_id,
                      core->buff_data[cur_bi].fb_id, DRM_MODE_PAGE_FLIP_EVENT, user_data)) {
    dlu_log_me(DLU_DANGER, "[x] drmModePageFlip: %s", strerror(errno));
    return false;
  }

  return true;
}

void *dlu_drm_gbm_bo_map(dlu_drm_core *core, uint32_t cur_bi, void **data, uint32_t flags) {
  return gbm_bo_map(core->buff_data[cur_bi].bo, 0, 0, core->output_data[core->buff_data[cur_bi].odid].mode.hdisplay,
         core->output_data[core->buff_data[cur_bi].odid].mode.vdisplay, flags, &core->buff_data[cur_bi].pitches[0], data);
}

void dlu_drm_gbm_bo_unmap(struct gbm_bo *bo, void *map_data) {
  gbm_bo_unmap(bo, map_data);
}

int dlu_drm_gbm_bo_write(struct gbm_bo *bo, const void *buff, size_t count) {
  return gbm_bo_write(bo, buff, count);
}
