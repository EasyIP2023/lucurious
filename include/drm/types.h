/**
* Parts of this file contain functionality similar to what is in kms-quads kms-quads.h:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms-quads.h
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
* Represents the values of an enum-type KMS property. These properties
* have a certain range of values you can use, exposed as strings from
* the kernel; userspace needs to look up the value that string
* corresponds to and use it.
*/
struct drm_property_enum_info {
  const char *name; /**< name as string (static, not freed) */
  bool valid; /**< true if value is supported; ignore if false */
  uint64_t value; /**< raw value */
};

/**
* Holds information on a DRM property, including its ID and the enum
* values it holds.
*
* DRM properties are allocated dynamically, and maintained as DRM objects
* within the normal object ID space; they thus do not have a stable ID
* to refer to. This includes enum values, which must be referred to by
* integer values, but these are not stable.
*
* drm_property_info allows a cache to be maintained where we can use
* enum values internally to refer to properties, with the mapping to DRM
* ID values being maintained internally.
*/
struct drm_property_info {
  const char *name; /**< name as string (static, not freed) */
  uint32_t prop_id; /**< KMS property object ID */
  unsigned int num_enum_values; /**< number of enum values */
  struct drm_property_enum_info *enum_values; /**< array of enum values */
};

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

  /**
  * Output Data struct contains information of a given
  * Plane, CRTC, Encoder, Connector pair
  */
  uint32_t dcc; /* Device connector count */
  struct _output_data {
    uint64_t refresh; /* Refresh rate for a pait */
    uint32_t pp_id;   /* Primary Plane ID */
    uint32_t crtc_id; /* CTRC ID */
    uint32_t conn_id; /* Connector ID */
    uint32_t enc_id;  /* Keeping encoder ID just because */

    /**
    * Encoders are deprecated and unused KMS objects
    * The plane -> CRTC -> connector chain construct
    */
    drmModePlane *plane;
    drmModeCrtc *crtc;
    drmModeEncoder *enc;
    drmModeConnector *conn;
  } *output_data;
};


typedef struct _dlu_drm_core {
  struct _dlu_device device;
  struct _dlu_logind session;
} dlu_drm_core;

#endif
