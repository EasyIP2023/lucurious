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

#define LUCUR_WAYLAND_API
#define LUCUR_DRM_EXEC_API
#include <lucom.h>

#include <fcntl.h>
#include <xf86drmMode.h>

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

int wlu_print_dconf_info(const char *gpu) {
  int fd = open(gpu, O_RDONLY);
  if (fd == NEG_ONE) {
    wlu_print_msg(WLU_DANGER, "[x] open: %s\n", strerror(errno));
    return NEG_ONE;  
  }

  wlu_print_msg(WLU_WARNING, "For more info see inside drmModeGetResources(3)\n\n");
  drmModeRes *dms = drmModeGetResources(fd);

  if (dms->count_crtcs)
    wlu_print_msg(WLU_SUCCESS, "\t\tAvailable CRTCs\n");
  for (int i=0; i < dms->count_crtcs; i++)
    wlu_print_msg(WLU_INFO, "\t\t\t%d\n", dms->crtcs[i]);

  if (dms->count_fbs)
    wlu_print_msg(WLU_SUCCESS, "\n\t\tAloocated Framebuffers\n");
  for (int i=0; i < dms->count_fbs; i++)
    wlu_print_msg(WLU_INFO, "\t\t\t%d\n", dms->fbs[i]);

  wlu_print_msg(WLU_SUCCESS, "\n\tConnector ID\tConnector Name\tCTRC ID\t  Framebuffer\n");
  for (int i=0; i < dms->count_connectors; i++) {
    drmModeConnector *c = drmModeGetConnector(fd, dms->connectors[i]);
    wlu_print_msg(WLU_INFO,  "\t     %d      \t    %s", c->connector_id, ouput_devices(c->connector_type));
    if (c->encoder_id) {
      drmModeEncoder *e = drmModeGetEncoder(fd, c->encoder_id);
      drmModeCrtc *crtc = drmModeGetCrtc(fd, e->crtc_id);
      wlu_print_msg(WLU_INFO,"\t\t  %d", e->crtc_id);
      wlu_print_msg(WLU_INFO,"\t      %d\n", crtc->buffer_id);
      drmModeFreeCrtc(crtc);
      drmModeFreeEncoder(e);
    }
    drmModeFreeConnector(c);
  }

  puts("");
  drmModeFreeResources(dms);
  close(fd);
  return 0;
}
