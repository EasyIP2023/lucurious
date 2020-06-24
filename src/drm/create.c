/**
* Parts of this file contain functionality similar to what is in kms-quads device.c, kms.c, and input.c:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/device.c
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/input.c
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

/* Open a single DRM device. Check to see if the node is a good candidate for usage */
static bool check_if_good_candidate(dlu_drm_core *core, const char *device_name) {
  drm_magic_t magic;
  uint64_t cap = 0;
  int err = 0;

  /**
  * Open the device and ensure we have support for universal planes and
  * atomic modesetting. This function updates the kmsfd struct member
  */
  if (!logind_take_device(DLU_KMS_FD, core, device_name)) goto close_kms;

  /**
  * In order to drive KMS, we need to be 'master'. This should already
  * have happened for us thanks to being root and the first client.
  * There can only be one master at a time, so this will fail if
  * (e.g.) trying to run this test whilst a graphical session is
  * already active on the current VT.
  */
  if (drmGetMagic(core->device.kmsfd, &magic) != 0 || drmAuthMagic(core->device.kmsfd, magic) != 0) {
    dlu_log_me(DLU_DANGER, "[x] KMS node '%s' is not master", device_name);
    goto close_kms;
  } else {
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' is master", device_name);
  }

  /* Universal planes means exposing primary & cursor as proper plane objects */
  err = drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
  err |= drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_ATOMIC, 1);
  if (err < 0) {
    dlu_log_me(DLU_DANGER, "[x] KMS node '%s' has no support for universal planes or KMS atomic", device_name);
    goto close_kms;
  } else {
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' has support for universal planes and or KMS atomic", device_name);
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
  logind_release_device(DLU_KMS_FD, core);
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

bool dlu_drm_create_kms_node(dlu_drm_core *core, const char *preferred_dev) {
  bool ret = false;

  if (preferred_dev) {
    ret = check_if_good_candidate(core, preferred_dev);
    if (ret) {
      dlu_log_me(DLU_SUCCESS, "Your KMS node '%s' is suitable!!", preferred_dev);
      return ret;
    }
  }

  drmDevicePtr *devices = NULL;
  uint32_t num_dev = drmGetDevices2(0, NULL, 0);
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

  char *kms_node = NULL;
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

static int open_restricted(const char *path, int UNUSED flags, void *user_data) {
  return logind_take_device(DLU_INP_FD, (dlu_drm_core *) user_data, path);
}

static void close_restricted(int UNUSED fd, void *user_data) {
  logind_release_device(DLU_INP_FD, (dlu_drm_core *) user_data);
}

static const struct libinput_interface libinput_impl = {
  .open_restricted = open_restricted,
  .close_restricted = close_restricted
};

bool dlu_drm_create_input_handle(dlu_drm_core *core) {

  core->input.udev = udev_new();
  if (!core->input.udev) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a udev context");
    return false;
  }

  core->input.inp = libinput_udev_create_context(&libinput_impl, core, core->input.udev);
  if (!core->input.inp) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create libinput context");
    return false;
  }

  if (libinput_udev_assign_seat(core->input.inp, "seat0") < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to assign seat0 to instance of libinput");
    return false;
  }

  return true;
}

bool dlu_drm_create_gbm_device(dlu_drm_core *core) {
  core->device.gbm_device = gbm_create_device(core->device.kmsfd);
  if (core->device.gbm_device) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a gbm device");
    return false;
  }

  return true;
}

bool dlu_drm_create_buffer(dlu_drm_core *core, uint32_t UNUSED cur_bi) {

  if (!core->buff_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_BUFF_DATA"); return false; }

  return true;
}

bool dlu_drm_create_gbm_bo(dlu_drm_bo_type type, dlu_drm_core *core, uint32_t UNUSED cur_bi) {

  if (!core->buff_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_BUFF_DATA"); return false; }

  switch (type) {
    case DLU_DRM_BO: break;
    case DLU_DRM_BO_WITH_MODIFIERS: break;
    default: break;
  }

  return true;
}