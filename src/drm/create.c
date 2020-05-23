/**
* Parts of this file contain functionality similar to what is in kms-quads device.c:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/device.c
*/

#define LUCUR_DRM_API
#include <lucom.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/vt.h>

/* Open a single KMS device. Check to see if the node is a good candidate for usage */
static bool check_if_good_candidate(dlu_drm_core *core, const char *filename) {
  drm_magic_t magic;
  uint64_t cap = 0;
  int err = 0;

  /**
  * Open the device and ensure we have support for universal planes and
  * atomic modesetting. This function updates the kmsfd struct member
  */
  if (!logind_take_device(core, filename)) goto close_kms;

  /**
  * In order to drive KMS, we need to be 'master'. This should already
  * have happened for us thanks to being root and the first client.
  * There can only be one master at a time, so this will fail if
  * (e.g.) trying to run this test whilst a graphical session is
  * already active on the current VT.
  */
  if (drmGetMagic(core->device.kmsfd, &magic) != 0 ||
      drmAuthMagic(core->device.kmsfd, magic) != 0) {
    dlu_log_me(DLU_WARNING, "[x] KMS device %s is not master", filename);
    goto exit_failure;
  }

  err = drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
  err |= drmSetClientCap(core->device.kmsfd, DRM_CLIENT_CAP_ATOMIC, 1);
  if (err < 0) {
    dlu_log_me(DLU_WARNING, "[x] no support for universal planes or atomic");
		goto exit_failure;
  }

  err = drmGetCap(core->device.kmsfd, DRM_CAP_ADDFB2_MODIFIERS, &cap);
  if (err || cap == 0) {
    dlu_log_me(DLU_WARNING, "[x] device doesn't support framebuffer modifiers");
    goto exit_failure;
  }

  cap = 0;
  err = drmGetCap(core->device.kmsfd, DRM_CAP_TIMESTAMP_MONOTONIC, &cap);
  if (err || cap == 0) {
    dlu_log_me(DLU_WARNING, "[x] device doesn't support clock monotonic timestamps");
    goto exit_failure;
  }

  return true;

exit_failure:
  release_session_control(core);
close_kms:
  close(core->device.kmsfd);
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
* This also lets a process handle it own input
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

  bool ret = false;
  for (uint32_t i = 0; i < num_dev; i++) {
    /**
    * We need /dev/dri/cardN nodes for modesetting, not render
    * nodes; render nodes are only used for GPU rendering, and
    * control nodes are totally useless. Primary nodes are the
    * only ones which let us control KMS.
    */
    if (!(devices[i]->available_nodes & (1 << DRM_NODE_PRIMARY))) continue;

    ret = check_if_good_candidate(core, devices[i]->nodes[DRM_NODE_PRIMARY]);
    if (ret) {
      dlu_log_me(DLU_SUCCESS, "Suitable KMS node found!!");
      break;
    }
  }

  drmFreeDevices(devices, num_dev);

  return ret;
}