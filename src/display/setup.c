/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2021 Vincent Davis Jr.
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

#define LUCUR_DISPLAY_API
#include <lucom.h>

dlu_disp_core *dlu_disp_init_core() {
  dlu_disp_core *core = dlu_alloc(DLU_SMALL_BLOCK_PRIV, sizeof(dlu_disp_core));
  if (!core) { PERR(DLU_ALLOC_FAILED, 0, NULL); return core; };
  core->device.vtfd = core->device.kmsfd = UINT32_MAX;
  return core;
}

void dlu_disp_freeup_core(dlu_disp_core *core) {

  if (core->oc_data) {
    uint32_t j, i;
    for (i = 0; i < core->odc; i++) {
      free(core->oc_data[i].modifiers);
      if (core->oc_data[i].mode_blob_id != 0) {
        struct drm_mode_destroy_blob destroy;
        memset(&destroy, 0, sizeof(destroy));
        destroy.blob_id = core->oc_data[i].mode_blob_id;
        ioctl(core->device.kmsfd, DRM_IOCTL_MODE_DESTROYPROPBLOB, &destroy);
      }

      // Reset crtc info
      //drmModeSetCrtc(core->device.kmsfd, core->oc_data[i].crtc->crtc_id, core->oc_data[i].crtc->buffer_id, core->oc_data[i].crtc->x,
      //               core->oc_data[i].crtc->y, &core->oc_data[i].conn_id, 1, &core->oc_data[i].crtc->mode);
      for (j = 0; j < DLU_DISPLAY_PLANE_TYPE__CNT; j++)
        free(core->oc_data[i].props.planes[j].enum_values);
      for (j = 0; j < DLU_DISPLAY_CRTC__CNT; j++)
        free(core->oc_data[i].props.crtcs[j].enum_values);
      for (j = 0; j < DLU_DISPLAY_CONNECTOR__CNT; j++)
        free(core->oc_data[i].props.conns[j].enum_values);
    }
  }

  if (core->dfb_data) {
    for (uint32_t i = 0; i < core->odbc; i++) {
      for (uint32_t j = 0; j < core->dfb_data[i].num_planes; j++)
        close(core->dfb_data[i].dma_buf_fds[j]);
      if (core->dfb_data[i].fb_id)
        ioctl(core->device.kmsfd, DRM_IOCTL_MODE_RMFB, &core->dfb_data[i].fb_id);
      if (core->dfb_data[i].bo)
        gbm_bo_destroy(core->dfb_data[i].bo);
    }
  }

  if (core->device.gbm_device)
    gbm_device_destroy(core->device.gbm_device);

  if (core->device.udev)
    udev_unref(core->device.udev);

//  if (core->input.inp)
//    libinput_unref(core->input.inp);
  if (core->device.vtfd != UINT32_MAX) {
    dlu_kms_vt_reset(core);
    close(core->device.vtfd);
  }

  /* Release logind session/devices and memory */ 
  if (core->session.path || core->session.id) {
    logind_release_device(core->device.kmsfd, core); /* Closes kmsfd  */
    release_session_control(core);
    free(core->session.path);
    free(core->session.id);
  }
}

