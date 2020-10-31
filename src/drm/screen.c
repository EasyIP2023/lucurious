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

/* Need to add more IF Statement checks in file to ensure proper data is created and passed */

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

  core->output_data[core->buff_data[cur_bi].odid].pflip = true;

  return true;
}

int dlu_drm_do_handle_event(int fd, drmEventContext *ev) {
  return drmHandleEvent(fd, ev);
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

/* Set property of a connector inside of a atomic request */
static int add_conn_prop(dlu_drm_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_drm_connector_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.conn[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].conn_id, core->output_data[cur_od].props.conn[prop].prop_id, val);
  /*
  dlu_log_me(DLU_INFO, "[CONN:%lu] %lu (%s) -> %llu (0x%llx)",
                       (unsigned long) core->output_data[cur_od].conn_id,
                       (unsigned long) core->output_data[cur_od].props.conn[prop].prop_id,
                       core->output_data[cur_od].props.conn[prop].name,
                       (unsigned long long) val, (unsigned long long) val);
  */
  return (ret <= 0) ? -1 : 0;
}

/* Set property of a CRTC inside of a atomic request */
static int add_crtc_prop(dlu_drm_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_drm_crtc_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.crtc[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].crtc_id, core->output_data[cur_od].props.crtc[prop].prop_id, val);
  /*
  dlu_log_me(DLU_INFO, "[CONN:%lu] %lu (%s) -> %llu (0x%llx)",
                       (unsigned long) core->output_data[cur_od].crtc_id,
                       (unsigned long) core->output_data[cur_od].props.crtc[prop].prop_id,
                       core->output_data[cur_od].props.crtc[prop].name,
                       (unsigned long long) val, (unsigned long long) val);
  */
  return (ret <= 0) ? -1 : 0;
}

/* Set property of a plane inside of a atomic request */
static int add_plane_prop(dlu_drm_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_drm_plane_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.plane[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].pp_id, core->output_data[cur_od].props.plane[prop].prop_id, val);
  /*
  dlu_log_me(DLU_INFO, "[CONN:%lu] %lu (%s) -> %llu (0x%llx)",
                       (unsigned long) core->output_data[cur_od].pp_id,
                       (unsigned long) core->output_data[cur_od].props.plane[prop].prop_id,
                       core->output_data[cur_od].props.plane[prop].name,
                       (unsigned long long) val, (unsigned long long) val);
  */

  return (ret <= 0) ? -1 : 0;
}

bool dlu_drm_do_atomic_req(dlu_drm_core *core, uint32_t cur_bd, drmModeAtomicReq *req) {
  int ret;

  /* Output device */
  uint32_t cur_od = core->buff_data[cur_bd].odid;
  uint32_t width = core->output_data[cur_od].mode.hdisplay;
  uint32_t height = core->output_data[cur_od].mode.vdisplay;

  // dlu_log_me(DLU_WARNING, "[%s] atomic state for commit:", core->output_data[cur_od].name);

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_CRTC_ID, core->output_data[cur_od].crtc_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_FB_ID, core->buff_data[cur_bd].fb_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_SRC_X, 0);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_SRC_Y, 0);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_SRC_W, width << 16);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_SRC_H, height << 16);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_CRTC_X, 0);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_CRTC_Y, 0);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_CRTC_W, width);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DRM_PLANE_CRTC_H, height);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  /**
  * Changing any of these two properties requires the ALLOW_MODESET
  * flag to be set on the atomic commit.
  */
  ret = add_crtc_prop(core, cur_od, req, DLU_DRM_CRTC_MODE_ID, core->output_data[cur_od].mode_blob_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_crtc_prop: %s", strerror(errno)); return false; }

  ret = add_crtc_prop(core, cur_od, req, DLU_DRM_CRTC_ACTIVE, 1);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_crtc_prop: %s", strerror(errno)); return false; }

  ret = add_conn_prop(core, cur_od, req, DLU_DRM_CONNECTOR_CRTC_ID, core->output_data[cur_od].crtc_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_conn_prop: %s", strerror(errno)); return false; }

  return true;
}

bool dlu_drm_do_atomic_commit(dlu_drm_core *core, uint32_t cur_bd, drmModeAtomicReq *req, bool allow_modeset) {
  uint32_t flags = DRM_MODE_ATOMIC_NONBLOCK | DRM_MODE_PAGE_FLIP_EVENT;
  
  if (allow_modeset) /* If not set still works fine */
    flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;

  if (drmModeAtomicCommit(core->device.kmsfd, req, flags, core)) {
    dlu_log_me(DLU_DANGER, "[x] Atomic commit failed!!");
    return false;
  }

  core->output_data[core->buff_data[cur_bd].odid].pflip = true;

  return true;
}

drmModeAtomicReq *dlu_drm_do_atomic_alloc() { return drmModeAtomicAlloc(); }
void dlu_drm_do_atomic_free(drmModeAtomicReq *req) { drmModeAtomicFree(req); }
