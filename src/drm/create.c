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

bool dlu_drm_create_gbm_device(dlu_drm_core *core) {
  core->device.gbm_device = gbm_create_device(core->device.kmsfd);
  if (!core->device.gbm_device) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a gbm device");
    return false;
  }

  return true;
}

/**
* I decided not to create a gbm_surface object. Then create a gbm_bo from that surface.
* Like such:
* struct gbm_surface *gbm_surface = gbm_surface_create(gbm_device, mode_info.hdisplay, mode_info.vdisplay,
*                                   GBM_BO_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
* struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbm_surface);
*/
bool dlu_drm_create_fb(
  dlu_drm_bo_type type,
  dlu_drm_core *core,
  uint32_t cur_bi,
  uint32_t cur_od,
  uint32_t format,
  uint32_t depth,
  uint32_t bpp,
  uint32_t bo_flags,
  uint32_t flags
) {

  if (!core->buff_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_BUFF_DATA"); goto err_bo; }
  if (!core->device.gbm_device) { dlu_log_me(DLU_DANGER, "[x] GBM device not created");  goto err_bo; }

  core->buff_data[cur_bi].odid = cur_od;
  
  switch (type) {
    case DLU_DRM_GBM_BO:
      core->buff_data[cur_bi].bo = gbm_bo_create(core->device.gbm_device, core->output_data[cur_od].mode.hdisplay,
                                                 core->output_data[cur_od].mode.vdisplay, format, bo_flags); break;
    case DLU_DRM_GBM_BO_WITH_MODIFIERS:
      core->buff_data[cur_bi].bo = gbm_bo_create_with_modifiers(core->device.gbm_device, core->output_data[cur_od].mode.hdisplay, core->output_data[cur_od].mode.vdisplay,
                                                                format, core->output_data[cur_od].modifiers, core->output_data[cur_od].modifiers_cnt); break;
    default: break;
  }

  if (!core->buff_data[cur_bi].bo) {
    dlu_log_me(DLU_DANGER, "[x] failed to create gbm_bo with res %u x %u", core->output_data[cur_od].mode.hdisplay, core->output_data[cur_od].mode.vdisplay);
    goto err_bo;
  }

  core->buff_data[cur_bi].num_planes = gbm_bo_get_plane_count(core->buff_data[cur_bi].bo);
  core->buff_data[cur_bi].modifier = gbm_bo_get_modifier(core->buff_data[cur_bi].bo);
  core->buff_data[cur_bi].format = format;

  for (uint32_t i = 0; i < core->buff_data[cur_bi].num_planes; i++) {
    union gbm_bo_handle h;

    h = gbm_bo_get_handle_for_plane(core->buff_data[cur_bi].bo, i);
    if (!h.u32 || h.s32 == NEG_ONE) {
      dlu_log_me(DLU_DANGER, "[x] failed to get BO plane %d gem handle (modifier 0x%" PRIx64 ")", i, core->buff_data[cur_bi].modifier);
      goto err_bo;
    }

    core->buff_data[cur_bi].gem_handles[i] = h.u32;

    core->buff_data[cur_bi].pitches[i] = gbm_bo_get_stride_for_plane(core->buff_data[cur_bi].bo, i);
    if (!core->buff_data[cur_bi].pitches[i]) {
      dlu_log_me(DLU_DANGER, "[x] failed to get stride/pitch for BO plane %d (modifier 0x%" PRIx64 ")", i, core->buff_data[cur_bi].modifier);
      goto err_bo;
    }

    core->buff_data[cur_bi].offsets[i] = gbm_bo_get_offset(core->buff_data[cur_bi].bo, i);

    struct drm_prime_handle prime_request = {
      .handle = core->buff_data[cur_bi].gem_handles[i],
      .flags  = DRM_RDWR,
      .fd     = -1
    };

    /* Retrieve a fd for the GEM handle */
    if (ioctl(core->device.kmsfd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime_request) == NEG_ONE)  {
      dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
      goto err_bo;
    }

    core->buff_data[cur_bi].dma_buf_fds[i] = prime_request.fd;
  }

  /* Create actual framebuffer */
  switch (type) {
    case DLU_DRM_GBM_BO:
      if (drmModeAddFB(core->device.kmsfd, core->output_data[cur_od].mode.hdisplay, core->output_data[cur_od].mode.vdisplay, depth, bpp,
                       core->buff_data[cur_bi].pitches[0], core->buff_data[cur_bi].gem_handles[0], &core->buff_data[cur_bi].fb_id) == NEG_ONE) {
        dlu_log_me(DLU_DANGER, "[x] drmModeAddFB: %s", strerror(-errno));
        goto err_bo;
      }
      break;
    case DLU_DRM_GBM_BO_WITH_MODIFIERS:
      if (drmModeAddFB2WithModifiers(core->device.kmsfd, core->output_data[cur_od].mode.hdisplay, core->output_data[cur_od].mode.vdisplay,
                                     format, core->buff_data[cur_bi].gem_handles, core->buff_data[cur_bi].pitches, core->buff_data[cur_bi].offsets,
                                     core->output_data[cur_od].modifiers, &core->buff_data[cur_bi].fb_id, flags) == NEG_ONE) {
        dlu_log_me(DLU_DANGER, "[x] drmModeAddFB2WithModifiers: %s", strerror(-errno));
        goto err_bo;
      }
      break;
    default: break;
  }

  dlu_log_me(DLU_SUCCESS, "Successfully created gbm framebuffer");

  return true;
err_bo:
  return false;
}

static int open_restricted(const char *path, int UNUSED flags, void *user_data) {
   dlu_drm_core *core = (dlu_drm_core *) user_data;
  return logind_take_device(DLU_INP_FD, core, path) ? core->input.inpfd : -1;
}

static void close_restricted(int  fd, void *user_data) {
  logind_release_device(DLU_INP_FD, (dlu_drm_core *) user_data);
}

bool dlu_drm_create_input_handle(dlu_drm_core *core) {

  core->input.udev = udev_new();
  if (!core->input.udev) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a udev context");
    return false;
  }

  core->input.inp = libinput_udev_create_context(&(struct libinput_interface) { .open_restricted = open_restricted, .close_restricted = close_restricted } , core, core->input.udev);
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