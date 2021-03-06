/**
* Parts of this file contain functionality similar to what is in kms-quads kms-quads.h:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms-quads.h
* If you want a tutorial go to the link above
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

#ifndef DLU_DISPLAY_TYPES_H
#define DLU_DISPLAY_TYPES_H

/* These headers allow for the use of ioctl calls directly */
#include <drm.h>
#include <drm_fourcc.h>
#include <drm_mode.h>

/**
* These headers are DRM API user space headers. Mainly used
* for device and resource enumeration
*/
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <fcntl.h>
#include <sys/ioctl.h>

/* GBM allocates buffers that are used with KMS */
#include <gbm.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-login.h>

/* For input handling */
#include <libudev.h>
#include <libinput.h>

struct drm_prop_enum_info {
  const char *name; /**< name as string (static, not freed) */
  bool valid; /**< true if value is supported; ignore if false */
  uint64_t value; /**< raw value */
};

struct drm_prop_info {
  const char *name; /**< name as string (static, not freed) */
  uint32_t prop_id; /**< KMS property object ID */
  unsigned enum_values_cnt; /**< number of enum values */
  struct drm_prop_enum_info *enum_values; /**< array of enum values */
};

/**
* DLU_DISPLAY_PLANE_TYPE_PRIMARY: Store background image or graphics content
* DLU_DISPLAY_PLANE_TYPE_CURSOR: Used to display a cursor plane (mouse)
* DLU_DISPLAY_PLANE_TYPE_OVERLAY: Used to display any surface (window) over a background
*/
typedef enum _dlu_disp_plane_type {
  DLU_DISPLAY_PLANE_TYPE_PRIMARY,
  DLU_DISPLAY_PLANE_TYPE_CURSOR,
  DLU_DISPLAY_PLANE_TYPE_OVERLAY,
  DLU_DISPLAY_PLANE_TYPE__CNT
} dlu_disp_plane_type;

typedef enum _dlu_disp_connector_props {
  DLU_DISPLAY_CONNECTOR_EDID,
  DLU_DISPLAY_CONNECTOR_DPMS,
  DLU_DISPLAY_CONNECTOR_CRTC_ID,
  DLU_DISPLAY_CONNECTOR_NON_DESKTOP,
  DLU_DISPLAY_CONNECTOR__CNT
} dlu_disp_connector_props;

/* Display Power Management Signaling */
typedef enum _dlu_disp_dpms_state {
  DLU_DISPLAY_DPMS_STATE_OFF,
  DLU_DISPLAY_DPMS_STATE_ON,
  DLU_DISPLAY_DPMS_STATE__CNT
} dlu_disp_dpms_state;

typedef enum _dlu_disp_crtc_props {
  DLU_DISPLAY_CRTC_MODE_ID,
  DLU_DISPLAY_CRTC_ACTIVE,
  DLU_DISPLAY_CRTC_OUT_FENCE_PTR,
  DLU_DISPLAY_CRTC__CNT
} dlu_disp_crtc_props;

/* Properties attached to DRM planes */
typedef enum _dlu_disp_plane_props {
  DLU_DISPLAY_PLANE_TYPE,
  DLU_DISPLAY_PLANE_SRC_X,
  DLU_DISPLAY_PLANE_SRC_Y,
  DLU_DISPLAY_PLANE_SRC_W,
  DLU_DISPLAY_PLANE_SRC_H,
  DLU_DISPLAY_PLANE_CRTC_X,
  DLU_DISPLAY_PLANE_CRTC_Y,
  DLU_DISPLAY_PLANE_CRTC_W,
  DLU_DISPLAY_PLANE_CRTC_H,
  DLU_DISPLAY_PLANE_FB_ID,
  DLU_DISPLAY_PLANE_CRTC_ID,
  DLU_DISPLAY_PLANE_IN_FORMATS,
  DLU_DISPLAY_PLANE_IN_FENCE_FD,
  DLU_DISPLAY_PLANE__CNT
} dlu_disp_plane_props;

typedef enum _dlu_disp_bo_type {
  DLU_DISPLAY_GBM_BO,
  DLU_DISPLAY_GBM_BO_WITH_MODIFIERS
} dlu_disp_bo_type;

typedef struct _dlu_disp_fb_info {
  dlu_disp_bo_type type;
  uint32_t cur_odb;
  uint32_t depth;
  uint32_t bpp;
  uint32_t bo_flags;
  uint32_t format;
  uint32_t flags;
} dlu_disp_fb_info;

typedef struct _dlu_disp_device_info {
  uint32_t conn_idx;
  uint32_t enc_idx;
  uint32_t crtc_idx;
  uint32_t plane_idx;
  uint64_t refresh;
  char conn_name[32];
} dlu_disp_device_info;

typedef struct _dlu_disp_core {
 
  uint32_t odbc;
  struct _drm_buff_data {

    /* Output Device ID, Used by the API, No need to utilize member yourself */ 
    uint32_t odid;
    struct gbm_bo *bo;

    /**
    * The format of an image details how each pixel color channels is laid out in memory: (i.e. RAM, VRAM, etc...)
    * So basically the width in bits, type, and ordering of each pixels color channels.
    * The modifier details information on how pixels should be within a buffer for different types
    * operations such as scan out or rendering. (i.e linear, tiled, compressed, etc...)
    */
    uint32_t format;
    uint64_t modifier;

    /* true if a buffer (scan out buffer) is currently owned by KMS. */ 
    bool in_use;

    uint32_t num_planes;
    unsigned pitches[4];
    unsigned offsets[4];
    unsigned gem_handles[4];
    int dma_buf_fds[4];

    /**
    * GEM buffers are wrapped by framebuffer. These framebuffers are passed to KMS for display.
    * These framebuffers contain metadata i.e image resolution 
    */    
    uint32_t fb_id;
  } *buff_data;

  /**
  * Output Data struct contains information of a given
  * Plane, CRTC, Encoder, Connector pair
  */
  uint32_t odc; /* Output data count */
  struct _output_data {
    /* Used to keep track of page flips */
    bool pflip;

    unsigned modifiers_cnt;
    uint64_t *modifiers;

    /* connector name */
    char name[32];

    uint32_t mode_blob_id;
    drmModeModeInfo mode;
    uint64_t refresh; /* Refresh rate for a pair store in nanoseconds */

    uint32_t pp_id;   /* Primary Plane ID */
    uint32_t crtc_id; /* CRTC ID */
    uint32_t conn_id; /* Connector ID */
    uint32_t enc_id;  /* Keeping encoder ID just because */

    /**
    * Encoders are deprecated and unused KMS objects
    * The Plane -> CRTC -> Encoder -> Connector chain construct
    */
    drmModePlane *plane;
    drmModeCrtc *crtc;
    drmModeEncoder *enc;
    drmModeConnector *conn;

    struct {
      struct drm_prop_info plane[DLU_DISPLAY_PLANE__CNT];
      struct drm_prop_info crtc[DLU_DISPLAY_CRTC__CNT];
      struct drm_prop_info conn[DLU_DISPLAY_CONNECTOR__CNT];
    } props;
  } *output_data;

  struct _device {
    /* KMS API Device node */
    uint32_t kmsfd;

    uint32_t vtfd; /* Virtual Terminal File Descriptor */
    uint32_t bkbm; /* Backup Keyboard mode */

    /* A GBM device is used to create gbm_bo (it's a buffer allocator) */
    struct gbm_device *gbm_device;
  } device;

  struct _session {
    /* For open D-Bus connection */
    sd_bus *bus;

    /* Session id and path */
    char *id, *path;

    bool has_drm;
  } session;

  struct _input {
    struct udev *udev;
    struct libinput *inp;
  } input;
} dlu_disp_core;

#endif
