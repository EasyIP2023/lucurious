/**
* Parts of this file contain functionality similar to what is in kms-quads device.c:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/device.c
* and
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
*/

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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/vt.h>

/* Open a single KMS device. Check to see if the node is a good candidate for usage */
static bool check_if_good_candidate(dlu_drm_core *core, const char *device_name) {
  drm_magic_t magic;
  uint64_t cap = 0;
  int err = 0;

  /**
  * Open the device and ensure we have support for universal planes and
  * atomic modesetting. This function updates the kmsfd struct member
  */
  if (!logind_take_device(core, device_name)) goto close_kms;

  /**
  * In order to drive KMS, we need to be 'master'. This should already
  * have happened for us thanks to being root and the first client.
  * There can only be one master at a time, so this will fail if
  * (e.g.) trying to run this test whilst a graphical session is
  * already active on the current VT.
  */
  if (drmGetMagic(core->device.kmsfd, &magic) != 0 ||
      drmAuthMagic(core->device.kmsfd, magic) != 0) {
    dlu_log_me(DLU_DANGER, "[x] KMS node '%s' is not master", device_name);
    goto close_kms;
  } else {
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' is master", device_name);
  }

  err = drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
  err |= drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_ATOMIC, 1);
  if (err < 0) {
    dlu_log_me(DLU_DANGER, "[x] KMS node '%s' has no support for universal planes or atomic", device_name);
    goto close_kms;
  }

  err = drmGetCap(core->device.kmsfd, DRM_CAP_ADDFB2_MODIFIERS, &cap);
  if (err || cap == 0)
    dlu_log_me(DLU_WARNING, "KSM node '%s' doesn't support framebuffer modifiers", device_name);
  else
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' supports framebuffer modifiers", device_name);

  cap = 0;
  err = drmGetCap(core->device.kmsfd, DRM_CAP_TIMESTAMP_MONOTONIC, &cap);
  if (err || cap == 0)
    dlu_log_me(DLU_WARNING, "KMS node '%s' doesn't support clock monotonic timestamps", device_name);
  else
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' supports monotonic clock", device_name);

  return true;

close_kms:
  logind_release_device(core);
  return false;
}

void dlu_drm_reset_vt(dlu_drm_core *core) {
  if (ioctl(core->device.vtfd, KDSKBMODE, core->device.bkbm) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    return;
  }

  if (ioctl(core->device.vtfd, KDSETMODE, KD_TEXT) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    return;
  }
}

/**
* Setting up VT/TTY so program runs in graphical mode.
* This also lets a process handle its own input
*/
bool dlu_drm_create_vt(dlu_drm_core *core) {
  int vt_num = 0, tty = 0;
  char tty_dev[10]; /* stores location, chars are /dev/tty#N */

  /* looking for a free virtual terminal (VT) that can be used */
  tty = open("/dev/tty0", O_WRONLY);
  if (tty == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] open: %s", strerror(errno));
    return false;
  }

  if (ioctl(tty, VT_OPENQRY, &vt_num) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    close(tty); return false;
  }

  close(tty);

  snprintf(tty_dev, sizeof(tty_dev), "/dev/tty%d", vt_num);

  core->device.vtfd = open(tty_dev, O_RDWR | O_NOCTTY);
  if (core->device.vtfd == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "open: %s", strerror(errno));
    return false;
  }

  /* Switching over to virtual terminal */
  if (ioctl(core->device.vtfd, VT_ACTIVATE, vt_num) == NEG_ONE ||
      ioctl(core->device.vtfd, VT_WAITACTIVE, vt_num) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    dlu_log_me(DLU_DANGER, "[x] failed to switch to VT %d", vt_num);
    return false;
  }

  dlu_log_me(DLU_SUCCESS, "VT %d is in use", vt_num);

  /**
  * Disable kernel keyboard processing:
  * 1. Back up the keybord mode for restoration purposes.
  * 2. Then disable keyboard
  */
  if (ioctl(core->device.vtfd, KDGKBMODE, &core->device.bkbm) == NEG_ONE ||
      ioctl(core->device.vtfd, KDSKBMODE, K_OFF) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    dlu_log_me(DLU_DANGER, "[x] Disabling TTY keyboard processing failed");
    return false;
  }

  dlu_log_me(DLU_WARNING, "Changing VT %d to graphics mode", vt_num);

  /**
  * Now changing the VT into graphics mode.
  * stoping the kernel from printing text
  */
  if (ioctl(core->device.vtfd, KDSETMODE, KD_GRAPHICS) == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
    return false;
  }

  dlu_log_me(DLU_SUCCESS, "VT successfully setup");

  return true;
}

bool dlu_drm_create_kms_node(dlu_drm_core *core) {
  drmDevicePtr *devices = NULL;
  uint32_t num_dev = 0;

  num_dev = drmGetDevices2(0, NULL, 0);
  if (!num_dev) {
    dlu_log_me(DLU_DANGER, "[x] drmGetDevices2: %s", strerror(-num_dev));
    dlu_log_me(DLU_DANGER, "[x] no available KMS nodes from /dev/dri/*");
    return false;
  }

  devices = alloca(num_dev * sizeof(drmDevicePtr));

  num_dev = drmGetDevices2(0, devices, num_dev);
  if (!num_dev) {
    dlu_log_me(DLU_DANGER, "[x] drmGetDevices2: %s", strerror(-num_dev));
    return false;
  }

  dlu_log_me(DLU_SUCCESS, "%d available KMS nodes", num_dev);

  bool ret = false; char *kms_node = NULL;
  for (uint32_t i = 0; i < num_dev; i++) {
    /**
    * We need /dev/dri/cardN nodes for modesetting, not render
    * nodes; render nodes are only used for GPU rendering, and
    * control nodes are totally useless. Primary nodes are the
    * only ones which let us control KMS.
    */
    if (!(devices[i]->available_nodes & (1 << DRM_NODE_PRIMARY))) continue;

    kms_node = devices[i]->nodes[DRM_NODE_PRIMARY];
    ret = check_if_good_candidate(core, kms_node);
    if (ret) {
      dlu_log_me(DLU_SUCCESS, "Suitable KMS node found!! '%s'", kms_node);
      break;
    }
  }

  drmFreeDevices(devices, num_dev);

  return ret;
}

bool dlu_drm_create_kms_output_data(
  dlu_drm_core *core,
  uint32_t odb,
  uint32_t conn_id_idx,
  uint32_t enc_id_idx,
  uint32_t crtc_id_idx,
  uint32_t plane_id_idx,
  uint64_t refresh
) {

  bool ret = true;

  if (core->device.kmsfd == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] There appears to be no available DRM device");
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_drm_create_kms_node()");
    goto exit_func;
  }

  if (!core->device.output_data) {
    PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_DATA");
    goto exit_func;
  }

  drmModeRes *dmr = drmModeGetResources(core->device.kmsfd);
  if (!dmr) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetResources: %s\n", strerror(errno));
    ret = false; goto exit_func;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(core->device.kmsfd);
  if (!pres) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlaneResources: %s\n", strerror(errno));
    ret = false; goto exit_free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_print_msg(DLU_DANGER, "[x] DRM device found is not a KMS node\n");
    ret = false; goto exit_free_plane_res;
  }

  core->device.output_data[odb].conn = drmModeGetConnector(core->device.kmsfd, dmr->connectors[conn_id_idx]);
  if (!core->device.output_data[odb].conn) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetConnector: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->device.output_data[odb].enc = drmModeGetEncoder(core->device.kmsfd, dmr->encoders[enc_id_idx]);
  if (!core->device.output_data[odb].enc) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetEncoder: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->device.output_data[odb].crtc = drmModeGetCrtc(core->device.kmsfd, dmr->crtcs[crtc_id_idx]);
  if (!core->device.output_data[odb].crtc) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetCrtc: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->device.output_data[odb].plane = drmModeGetPlane(core->device.kmsfd, pres->planes[plane_id_idx]);
  if (!core->device.output_data[odb].plane) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPlane: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->device.output_data[odb].refresh = millihz_to_nsec(refresh);

  /* This members are redundant and mainly for easy of access */
  core->device.output_data[odb].conn_id = core->device.output_data[odb].conn->connector_id;
  core->device.output_data[odb].enc_id  = core->device.output_data[odb].enc->encoder_id;
  core->device.output_data[odb].crtc_id = core->device.output_data[odb].crtc->crtc_id;
  core->device.output_data[odb].pp_id   = core->device.output_data[odb].crtc->buffer_id;
  core->device.output_data[odb].mode    = core->device.output_data[odb].crtc->mode;

  /**
  * Now creating MODE_ID blob
  * Go here for more information: https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
  */
  if (drmModeCreatePropertyBlob(core->device.kmsfd, &core->device.output_data[odb].mode, sizeof(drmModeModeInfo), &core->device.output_data[odb].mode_blob_id) < 0) {
    dlu_log_me(DLU_DANGER, "[x] drmModeCreatePropertyBlob: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  /**
  * Objects are now line up can now get their property lists from
  * KMS and use that to fill in the props structures so
  * we can more easily query and set them.
  */
  drmModeObjectProperties *props = drmModeObjectGetProperties(core->device.kmsfd, core->device.output_data[odb].pp_id, DRM_MODE_OBJECT_PLANE);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }


  drmModeFreeObjectProperties(props);
exit_free_plane_res:
  drmModeFreePlaneResources(pres);
exit_free_drm_res:
  drmModeFreeResources(dmr);
exit_func:
  return ret;
}

bool dlu_drm_create_gbm_device(dlu_drm_core *core) {
  core->device.gbm_device = gbm_create_device(core->device.kmsfd);
  if (core->device.gbm_device) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a gbm device");
    return false;
  }

  return true;
}
