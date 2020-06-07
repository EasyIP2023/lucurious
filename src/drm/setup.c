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

dlu_drm_core *dlu_drm_init_core() {
  dlu_drm_core *core = dlu_alloc(DLU_SMALL_BLOCK_PRIV, sizeof(dlu_drm_core));
  if (!core) { PERR(DLU_ALLOC_FAILED, 0, NULL); return core; };
  core->device.vtfd = core->device.kmsfd = UINT32_MAX;
  return core;
}

void dlu_drm_freeup_core(dlu_drm_core *core) {
  logind_release_device(DLU_KMS_FD, core);
  release_session_control(core);
  free(core->session.path);
  free(core->session.id);
  if (core->input.inp)
    libinput_unref(core->input.inp);
  if (core->input.udev)
    udev_unref(core->input.udev);
  if (core->device.vtfd != UINT32_MAX) {
    dlu_drm_reset_vt(core);
    close(core->device.vtfd);
  }
  if (core->session.bus)
    sd_bus_unref(core->session.bus);
  if (core->device.output_data) {
    uint32_t j, i;
    for (i = 0; i < core->device.odc; i++) {
      free(core->device.output_data[i].modifiers);
      if (core->device.output_data[i].conn)
        drmModeFreeConnector(core->device.output_data[i].conn);
      if (core->device.output_data[i].enc)
        drmModeFreeEncoder(core->device.output_data[i].enc);
      if (core->device.output_data[i].crtc)
        drmModeFreeCrtc(core->device.output_data[i].crtc);
      if (core->device.output_data[i].plane)
        drmModeFreePlane(core->device.output_data[i].plane);
      for (j = 0; j < DLU_DRM_PLANE_TYPE__CNT; j++)
        free(core->device.output_data[i].props.plane[j].enum_values);
      for (j = 0; j < DLU_DRM_CRTC__CNT; j++)
        free(core->device.output_data[i].props.crtc[j].enum_values);
      for (j = 0; j < DLU_DRM_CONNECTOR__CNT; j++)
        free(core->device.output_data[i].props.conn[j].enum_values);
    }
  }
  if (core->device.gbm_device)
    gbm_device_destroy(core->device.gbm_device);
}

