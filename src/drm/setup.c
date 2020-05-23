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
  dlu_drm_core *core = dlu_alloc(DLU_SMALL_BLOCK_SHARED, sizeof(dlu_drm_core));
  if (!core) { PERR(DLU_ALLOC_FAILED, 0, NULL); return core; };
  core->device.vtfd = core->device.kmsfd = UINT32_MAX;
  return core;
}

void dlu_drm_freeup_core(dlu_drm_core *core) {
  release_session_control(core);
  if (core->device.vtfd != UINT32_MAX) dlu_drm_reset_vt(core);
  if (core->session.bus) sd_bus_unref(core->session.bus);
  free(core->session.path);
  free(core->session.id);
  if (core->device.gbm_device) gbm_device_destroy(core->device.gbm_device);
  if (core->device.kmsfd != UINT32_MAX) close(core->device.kmsfd);
  if (core->device.vtfd != UINT32_MAX) close(core->device.vtfd);
  if (core->device.dmr) drmModeFreeResources(core->device.dmr);
}

