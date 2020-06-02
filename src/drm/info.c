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

/* Intentionally did not add plane freeing into this function */
static void free_drm_objs(drmModeConnector **conn, drmModeEncoder **enc, drmModeCrtc **crtc, drmModePlane **plane) {
  if (conn) { // Just an extra check
    if (*conn) {
      drmModeFreeConnector(*conn);
      *conn = NULL;
    }
  }

  if (enc) { // Just an extra check
    if (*enc) {
      drmModeFreeEncoder(*enc);
      *enc = NULL;
    }
  }

  if (crtc) { // Just an extra check
    if (*crtc) {
      drmModeFreeCrtc(*crtc);
      *crtc = NULL;
    }
  }

  if (plane) { // Just an extra check
    if (*plane) {
      drmModeFreePlane(*plane);
      *plane = NULL;
    }
  }
}

bool dlu_print_dconf_info() {
  bool ret = true;

  dlu_drm_core *core = dlu_drm_init_core();

  if (dlu_otba(DLU_DEVICE_OUTPUT_DATA, core, INDEX_IGNORE, 1)) goto exit_info;

  /* Exit if not in a tty */
  if (!dlu_drm_create_session(core)) { dlu_print_msg(DLU_WARNING, "Please run command with in a TTY"); goto exit_info; }
  if (!dlu_drm_create_kms_node(core)) { dlu_print_msg(DLU_WARNING, "Please run command with in a TTY"); goto exit_info; }

  drmModeRes *dmr = drmModeGetResources(core->device.kmsfd);
  if (!dmr) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetResources: %s\n", strerror(errno));
    ret = false; goto exit_info;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(core->device.kmsfd);
  if (!pres) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlaneResources: %s\n", strerror(errno));
    ret = false; goto free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_print_msg(DLU_DANGER, "[x] Not a proper KMS node\n");
    ret = false; goto free_plane_res;
  }

  drmModeConnector *conn = NULL; drmModeEncoder *enc = NULL;
  drmModeCrtc *crtc = NULL; drmModePlane *plane = NULL;
  uint32_t enc_crtc_id = 0, crtc_id = 0;
  uint32_t fb_id = 0;
  uint64_t refresh = 0;

  for (int i = 0; i < dmr->count_connectors; i++) {
    conn = drmModeGetConnector(core->device.kmsfd, dmr->connectors[i]);
    if (!conn) {
      dlu_print_msg(DLU_DANGER, "[x] drmModeGetConnector: %s\n", strerror(errno));
      free_drm_objs(&conn, &enc, &crtc, &plane);
      ret = false; goto free_plane_res;
    }

    /* Finding a encoder (a deprecated KMS object) for a given connector */
    for (int e = 0; e < dmr->count_encoders; e++) {
      if (dmr->encoders[e] == conn->encoder_id) {
        enc = drmModeGetEncoder(core->device.kmsfd, dmr->encoders[e]);
        if (!enc) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetEncoder: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          ret = false; goto free_plane_res;
        }

        dlu_print_msg(DLU_SUCCESS, "\n\t\tConnector INFO\n");
        dlu_print_msg(DLU_INFO, "\tConn ID   : %u\tConn Index : %u\n", conn->connector_id, i);
        dlu_print_msg(DLU_INFO, "\tConn Type : %u\tConn Name  : %s\n", conn->connector_type, ouput_devices(conn->connector_type_id));
        dlu_print_msg(DLU_INFO, "\tEnc ID    : %u\n", conn->encoder_id);

        dlu_print_msg(DLU_SUCCESS, "\n\t\tEncoder INFO\n");
        dlu_print_msg(DLU_INFO, "\tEnc  ID   : %u\tEnc  Index : %u\n", enc->encoder_id, e);
        dlu_print_msg(DLU_INFO, "\tCrtc ID   : %u\n", enc->crtc_id); enc_crtc_id = enc->crtc_id;
        drmModeFreeEncoder(enc); enc = NULL;
        break;
      }
    }

    /* Finding a crtc for the given encoder */
    for (int c = 0; c < dmr->count_crtcs; c++) {
      if (dmr->crtcs[c] == enc_crtc_id) {
        crtc = drmModeGetCrtc(core->device.kmsfd, dmr->crtcs[c]);
        if (!crtc) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetCrtc: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          ret = false; goto free_plane_res;
        }

        dlu_print_msg(DLU_SUCCESS, "\n\t\tCRTC INFO\n");
        dlu_print_msg(DLU_INFO, "\tCrtc ID   : %u\tCTRC Index : %u\n", crtc->crtc_id, c);
        dlu_print_msg(DLU_INFO, "\tFB ID     : %u\tmode valid : %u\n", crtc->buffer_id, crtc->mode_valid);
        dlu_print_msg(DLU_INFO, "\twidth     : %u\theight     : %u\n", crtc->width, crtc->height);
        /* DRM is supposed to provide a refresh interval, but often doesn't;
        * calculate our own in milliHz for higher precision anyway. */
        refresh = ((crtc->mode.clock * 1000000LL / crtc->mode.htotal) + (crtc->mode.vtotal / 2)) / crtc->mode.vtotal;
        crtc_id = crtc->crtc_id; fb_id = crtc->buffer_id;
        drmModeFreeCrtc(crtc); enc_crtc_id = UINT32_MAX;
        break;
      }
    }

    /* Only search for planes if a given CRTC has an encoder connected to it and a connector connected to that encoder */
    if (crtc) {
      for (uint32_t p = 0; p < pres->count_planes; p++) {
        plane = drmModeGetPlane(core->device.kmsfd, pres->planes[p]);
        if (!plane) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlane: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          ret = false; goto free_plane_res;
        }

        /* Check find primary plane for chosen crtc */
        if (plane->crtc_id == crtc_id && plane->fb_id == fb_id) {
          dlu_print_msg(DLU_SUCCESS, "\n\t\tPlane INFO\n");
          dlu_print_msg(DLU_INFO, "\tPLANE ID : %u\tPlane Index : %u\n", plane->plane_id, p);
          dlu_print_msg(DLU_INFO, "\tFB ID    : %u\tCRTC ID     : %u\n", plane->fb_id, plane->crtc_id);
          dlu_print_msg(DLU_INFO, "\tgamma sz : %u\tformats     : [", plane->gamma_size);
          for (uint32_t j = 0; j < plane->count_formats; j++)
            dlu_print_msg(DLU_INFO, "%u ", plane->formats[j]);
          dlu_print_msg(DLU_INFO, "]\n");
          dlu_print_msg(DLU_WARNING, "\n  Plane -> CRTC -> Encoder -> Connector Pair: %d\n", (i+1));
          dlu_print_msg(DLU_DANGER, "\n\tscreen refresh: %u\n", refresh); refresh = UINT64_MAX;
        }

        drmModeFreePlane(plane); plane = NULL;
      }

      /* Reset values */
      crtc = NULL; crtc_id = fb_id = 0;
    }

    drmModeFreeConnector(conn); conn = NULL;
  }

  fprintf(stdout, "\n");
free_plane_res:
  drmModeFreePlaneResources(pres);
free_drm_res:
  drmModeFreeResources(dmr);
exit_info:
  dlu_drm_freeup_core(core);
  return ret;
}
