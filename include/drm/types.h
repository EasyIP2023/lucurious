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

#ifndef DLU_DRM_TYPES_H
#define DLU_DRM_TYPES_H

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

/* GBM allocates buffers that are used with KMS */
#include <gbm.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-login.h>

/**
* DLU_DRM_PLANE_TYPE_PRIMARY: Store background image or graphics content
* DLU_DRM_PLANE_TYPE_CURSOR: Used to display a cursor plane (mouse)
* DLU_DRM_PLANE_TYPE_OVERLAY: Used to display any image (window) over a background
*/
typedef enum _dlu_drm_plane_type {
  DLU_DRM_PLANE_TYPE_PRIMARY = 0x0000,
  DLU_DRM_PLANE_TYPE_CURSOR = 0x0001,
  DLU_DRM_PLANE_TYPE_OVERLAY = 0x0002
} dlu_drm_plane_type;

typedef enum _dlu_drm_connector_props {
  DLU_DRM_CONNECTOR_EDID = 0x0000,
  DLU_DRM_CONNECTOR_DPMS = 0x0001,
  DLU_DRM_CONNECTOR_CRTC_ID = 0x0002,
  DLU_DRM_CONNECTOR_NON_DESKTOP = 0x0003
} dlu_drm_connector_props;

struct _dlu_logind {
  /* For open D-Bus connection */
  sd_bus *bus;

  /* Session id and path */
  char *id, *path;

  bool has_drm;
};

struct _dlu_device {
  /* KMS API Device node */
  uint32_t kmsfd;

  uint32_t vtfd; /* Virtual Terminal File Descriptor */
  uint32_t bkbm; /* Backup Keyboard mode */

  /* A GBM device is used to create gbm_bo (it's a buffer allocator) */
  struct gbm_device *gbm_device;

  /* create logind session */
  struct _dlu_logind *session;

  drmModeRes *dmr;
  uint32_t dpc; /* Device plane count */
  struct _plane_data {
    drmModePlane *plane;
  } *plane_data;
};


typedef struct _dlu_drm_core {
  struct _dlu_device device;
  struct _dlu_logind session;
} dlu_drm_core;

#endif
