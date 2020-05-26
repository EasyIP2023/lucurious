/**
* Parts of this file contain functionality similar to what is in kms-quads device.c:
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

/* Can find here https://code.woboq.org/linux/linux/include/uapi/drm/drm_mode.h.html */
static const char UNUSED *ouput_devices(uint32_t type) {
  switch (type) {
    case DRM_MODE_CONNECTOR_Unknown:     return "Unknown";
    case DRM_MODE_CONNECTOR_VGA:         return "VGA";
    case DRM_MODE_CONNECTOR_DVII:        return "DVI-I";
    case DRM_MODE_CONNECTOR_DVID:        return "DVI-D";
    case DRM_MODE_CONNECTOR_DVIA:        return "DVI-A";
    case DRM_MODE_CONNECTOR_Composite:   return "Composite";
    case DRM_MODE_CONNECTOR_SVIDEO:      return "SVIDEO";
    case DRM_MODE_CONNECTOR_LVDS:        return "LVDS";
    case DRM_MODE_CONNECTOR_Component:   return "Component";
    case DRM_MODE_CONNECTOR_9PinDIN:     return "DIN";
    case DRM_MODE_CONNECTOR_DisplayPort: return "DP";
    case DRM_MODE_CONNECTOR_HDMIA:       return "HDMI-A";
    case DRM_MODE_CONNECTOR_HDMIB:       return "HDMI-B";
    case DRM_MODE_CONNECTOR_TV:          return "TV";
    case DRM_MODE_CONNECTOR_eDP:         return "eDP";
    case DRM_MODE_CONNECTOR_VIRTUAL:     return "Virtual";
    case DRM_MODE_CONNECTOR_DSI:         return "DSI";
    default:                             return "Unknown";
  }
}

/* Intentionally did not add plane freeing into this function */
static void free_drm_objs(drmModeConnector **conn, drmModeEncoder **enc, drmModeCrtc **crtc) {
  if (conn) {
    if (*conn) {
      drmModeFreeConnector(*conn);
      *conn = NULL;
    }
  }

  if (enc) {
    if (*enc) {
      drmModeFreeEncoder(*enc);
      *enc = NULL;
    }
  }

  if (crtc) {
    if (*crtc) {
      drmModeFreeCrtc(*crtc);
      *crtc = NULL;
    }
  }
}

bool dlu_print_dconf_info(const char *gpu) {
  uint32_t drmfd = UINT32_MAX;
  bool ret = true;

  drmfd = open(gpu, O_RDONLY);
  if (drmfd == UINT32_MAX) {
    dlu_print_msg(DLU_DANGER, "[x] open: %s\n", strerror(errno));
    ret = false; goto exit_info;
  }

  drmModeRes *dmr = drmModeGetResources(drmfd);
  if (!dmr) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetResources: %s\n", strerror(errno));
    ret = false; goto exit_info;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(drmfd);
  if (!pres) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlaneResources: %s\n", strerror(errno));
    ret = false; goto free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_print_msg(DLU_DANGER, "[x] Device '%s' is not a KMS node\n", gpu);
    ret = false; goto free_plane_res;
  }

  struct _ouput_data {
    uint64_t refresh;

    uint32_t conn_idx;
    drmModeConnector *conn;

    uint32_t enc_idx;
    drmModeEncoder *enc;

    uint32_t crtc_idx;
    drmModeCrtc *crtc;

    uint32_t plane_idx;
    drmModePlane *plane;
  } *output_data;

  output_data = alloca(dmr->count_connectors * sizeof(struct _ouput_data));

  drmModeConnector *conn = NULL;
  drmModeEncoder *enc = NULL;
  drmModeCrtc *crtc = NULL;
  drmModePlane *plane = NULL;
  uint32_t odb = 0; /* output_data buffer */

  uint32_t c = 0, p = 0; int e = 0;
  for (int i = 0; i < dmr->count_connectors; i++) {
    conn = drmModeGetConnector(drmfd, dmr->connectors[i]);
    if (!conn) {
      dlu_print_msg(DLU_DANGER, "[x] drmModeGetConnector: %s", strerror(errno));
      ret = false; goto free_objs;
    }

    /* Continue to the next connector if no encoder found */
    if (conn->encoder_id == 0) { free_drm_objs(&conn, NULL, NULL); continue; }

    /* Finding the encoder (a deprecated KMS object) for a given connector */
    for (; e < dmr->count_encoders; e++) {
      if (dmr->encoders[e] == conn->encoder_id) {
        enc = drmModeGetEncoder(drmfd, dmr->encoders[e]);
        if (!enc) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetEncoder: %s", strerror(errno));
          free_drm_objs(&conn, NULL, NULL);
          ret = false; goto free_objs;
        }
        break;
      }
    }

    /* Continue to the next connector if no crtc found */
    if (enc->crtc_id == 0) { free_drm_objs(&conn, &enc, NULL); continue; }

    for (; dmr->count_crtcs; c++) {
      if (dmr->crtcs[c] == enc->crtc_id) {
        crtc = drmModeGetCrtc(drmfd, dmr->crtcs[c]);
        if (!crtc) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetCrtc: %s", strerror(errno));
          free_drm_objs(&conn, &enc, NULL);
          ret = false; goto free_objs;
        }
        break;
      }
    }

    /* Ensure the CRTC is active. buffer_id = framebuffer_id */
    if (crtc->buffer_id == 0) { free_drm_objs(&conn, &enc, &crtc); continue; }

    for (; p < pres->count_planes; p++) {
      plane = drmModeGetPlane(drmfd, pres->planes[p]);
      if (!plane) {
        dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlane: %s", strerror(errno));
        free_drm_objs(&conn, &enc, &crtc);
        ret = false; goto free_objs;
      }

      if (plane->crtc_id == crtc->crtc_id && plane->fb_id == crtc->buffer_id)
        break;

      drmModeFreePlane(plane); plane = NULL;
    }

    /**
    * DRM is supposed to provide a refresh interval, but often doesn't;
    * calculate our own in milliHz for higher precision anyway.
    */
    output_data[odb].refresh = ((crtc->mode.clock * 1000000LL / crtc->mode.htotal) + (crtc->mode.vtotal / 2)) / crtc->mode.vtotal;
    output_data[odb].conn = conn;   output_data[odb].conn_idx = i;
    output_data[odb].enc = enc;     output_data[odb].enc_idx = e;
    output_data[odb].crtc = crtc;   output_data[odb].crtc_idx = c;
    output_data[odb].plane = plane; output_data[odb].plane_idx = p;
    odb++; e = c = p = 0;
    dlu_print_msg(DLU_SUCCESS, "Found Plane -> CTRC -> Encoder -> Connector pair");
  }

  fprintf(stdout, "\n");
free_objs:
  for (uint32_t i = 0; i < odb; i++) {
    if (output_data[i].conn)
      drmModeFreeConnector(output_data[i].conn);
    if (output_data[i].enc)
      drmModeFreeEncoder(output_data[i].enc);
    if (output_data[i].crtc)
      drmModeFreeCrtc(output_data[i].crtc);
    if (output_data[i].plane)
      drmModeFreePlane(output_data[i].plane);
  }
free_plane_res:
  drmModeFreePlaneResources(pres);
free_drm_res:
  drmModeFreeResources(dmr);
  close(drmfd);
exit_info:
  return ret;
}
