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
static const char *ouput_devices(uint32_t type) {
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

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 ||
      dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_print_msg(DLU_DANGER, "device '%s' is not a KMS node\n", gpu);
    ret = false; goto free_plane_res;
  }

  if (dmr->count_crtcs)
    dlu_print_msg(DLU_SUCCESS, "\t\tAvailable CRTCs\n");
  for (int i=0; i < dmr->count_crtcs; i++)
    dlu_print_msg(DLU_INFO, "\t\t\t%d\n", dmr->crtcs[i]);

  if (dmr->count_fbs)
    dlu_print_msg(DLU_SUCCESS, "\n\t\tAllocated Framebuffers\n");
  for (int i=0; i < dmr->count_fbs; i++)
    dlu_print_msg(DLU_INFO, "\t\t\t%d\n", dmr->fbs[i]);

  dlu_print_msg(DLU_SUCCESS, "\n\tConnector ID\tConnector Name\tCTRC ID\t  Framebuffer\n");
  for (int i=0; i < dmr->count_connectors; i++) {
    drmModeConnector *c = drmModeGetConnector(drmfd, dmr->connectors[i]);
    dlu_print_msg(DLU_INFO,  "\t     %d      \t    %s", c->connector_id, ouput_devices(c->connector_type));
    if (c->encoder_id) {
      drmModeEncoder *e = drmModeGetEncoder(drmfd, c->encoder_id);
      drmModeCrtc *crtc = drmModeGetCrtc(drmfd, e->crtc_id);
      dlu_print_msg(DLU_INFO, "\t\t  %d", e->crtc_id);
      dlu_print_msg(DLU_INFO, "\t      %d\n", crtc->buffer_id);
      drmModeFreeCrtc(crtc);
      drmModeFreeEncoder(e);
    }
    drmModeFreeConnector(c);
  }

  /* Eventually want to print out plane information */
  dlu_print_msg(DLU_SUCCESS, "\n\n\t\tPlane Info\n", pres->count_planes);
  dlu_print_msg(DLU_WARNING, "      Total planes for '%s': %d\n", gpu, pres->count_planes);
  dlu_print_msg(DLU_SUCCESS, "\n\tPlane Formats\tPlane ID   Gamma size\n");
  drmModePlane **planes = alloca(pres->count_planes);
  for (uint32_t i = 0; i < pres->count_planes; i++) {
    planes[i] = drmModeGetPlane(drmfd, pres->planes[i]);
    if (!planes[i]) { ret = false; goto free_plane_res; }

    fprintf(stdout, "\t");
    for (uint32_t j = 0; j < planes[i]->count_formats; j++)
      dlu_print_msg(DLU_INFO, "%d ");
    dlu_print_msg(DLU_INFO, "\t  %d", planes[i]->plane_id);
    dlu_print_msg(DLU_INFO, "\t\t%d\n", planes[i]->gamma_size);
  }

  fprintf(stdout, "\n");
//free_planes:
  for (uint32_t i = 0; i < pres->count_planes; i++)
    drmModeFreePlane(planes[i]);
free_plane_res:
  drmModeFreePlaneResources(pres);
free_drm_res:
  drmModeFreeResources(dmr);
  close(drmfd);
exit_info:
  return ret;
}
