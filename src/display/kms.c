/**
* Parts of this file contain functionality similar to what is in kms-quads device.c and kms.c:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/device.c
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
*/

#define LUCUR_DISPLAY_API
#include <lucom.h>

#include <linux/kd.h>
#include <linux/vt.h>

/* All properties and enum values supported one needs to */
static struct drm_prop_enum_info plane_type_enums[] = {
  [DLU_DISPLAY_PLANE_TYPE_PRIMARY] = { .name = "Primary" },
  [DLU_DISPLAY_PLANE_TYPE_OVERLAY] = { .name = "Overlay" },
  [DLU_DISPLAY_PLANE_TYPE_CURSOR]  = { .name = "Cursor"  }
};

static const struct drm_prop_info plane_props[] = {
  [DLU_DISPLAY_PLANE_TYPE] = {
    .name = "type",
    .enum_values = plane_type_enums,
    .enum_values_cnt = DLU_DISPLAY_PLANE_TYPE__CNT,
  },
  [DLU_DISPLAY_PLANE_SRC_X] = { .name = "SRC_X" },
  [DLU_DISPLAY_PLANE_SRC_Y] = { .name = "SRC_Y" },
  [DLU_DISPLAY_PLANE_SRC_W] = { .name = "SRC_W" },
  [DLU_DISPLAY_PLANE_SRC_H] = { .name = "SRC_H" },
  [DLU_DISPLAY_PLANE_CRTC_X] = { .name = "CRTC_X" },
  [DLU_DISPLAY_PLANE_CRTC_Y] = { .name = "CRTC_Y" },
  [DLU_DISPLAY_PLANE_CRTC_W] = { .name = "CRTC_W" },
  [DLU_DISPLAY_PLANE_CRTC_H] = { .name = "CRTC_H" },
  [DLU_DISPLAY_PLANE_FB_ID] = { .name = "FB_ID" },
  [DLU_DISPLAY_PLANE_CRTC_ID] = { .name = "CRTC_ID", },
  [DLU_DISPLAY_PLANE_IN_FORMATS] = { .name = "IN_FORMATS" },
  [DLU_DISPLAY_PLANE_IN_FENCE_FD] = { .name = "IN_FENCE_FD" }
};

/* Display Power Management Signaling */
static struct drm_prop_enum_info dpms_state_enums[] = {
  [DLU_DISPLAY_DPMS_STATE_OFF] = { .name = "Off" },
  [DLU_DISPLAY_DPMS_STATE_ON] = { .name = "On" }
};

static const struct drm_prop_info conn_props[] = {
  [DLU_DISPLAY_CONNECTOR_EDID] = { .name = "EDID" },
  [DLU_DISPLAY_CONNECTOR_DPMS] = {
    .name = "DPMS",
    .enum_values = dpms_state_enums,
    .enum_values_cnt = DLU_DISPLAY_DPMS_STATE__CNT,
  },
  [DLU_DISPLAY_CONNECTOR_CRTC_ID] = { .name = "CRTC_ID" },
  [DLU_DISPLAY_CONNECTOR_NON_DESKTOP] = { .name = "non-desktop" }
};

static const struct drm_prop_info crtc_props[] = {
  [DLU_DISPLAY_CRTC_MODE_ID] = { .name = "MODE_ID" },
  [DLU_DISPLAY_CRTC_ACTIVE] = { .name = "ACTIVE" },
  [DLU_DISPLAY_CRTC_OUT_FENCE_PTR] = { .name = "OUT_FENCE_PTR" }
};

/* Can find here https://code.woboq.org/linux/linux/include/uapi/drm/drm_mode.h.html */
static const char *conn_types(uint32_t type) {
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

static bool drm_prop_info_populate(
  dlu_disp_core *core,
  const struct drm_prop_info *src,
  struct drm_prop_info *dst,
  unsigned int num_infos,
  drmModeObjectProperties *props
) {

  drmModePropertyRes *prop;
  unsigned i, j;

  for (i = 0; i < num_infos; i++) {
    dst[i].name = src[i].name;
    dst[i].prop_id = 0;
    dst[i].enum_values_cnt = src[i].enum_values_cnt;

    if (dst[i].enum_values_cnt == 0) continue;

    dst[i].enum_values = calloc(src[i].enum_values_cnt, sizeof(*dst[i].enum_values));
    if (!dst[i].enum_values) {
      dlu_log_me(DLU_DANGER, "[x] calloc: %s", strerror(errno));
      return false;
    }

    for (j = 0; j < dst[i].enum_values_cnt; j++) {
      dst[i].enum_values[j].name = src[i].enum_values[j].name;
      dst[i].enum_values[j].valid = false;
    }
  }

  for (i = 0; i < props->count_props; i++) {
    unsigned int k;

    prop = drmModeGetProperty(core->device.kmsfd, props->props[i]);
    if (!prop) continue;

    for (j = 0; j < num_infos; j++) {
      if (!strcmp(prop->name, dst[j].name))
        break;
    }

    /* We don't know/care about this property. */
    if (j == num_infos) {
      drmModeFreeProperty(prop);
      prop = NULL;
      continue;
    }

    dst[j].prop_id = props->props[i];

    /* ensure we don't get mixed up between enum and normal properties. */
    if (!!(prop->flags & DRM_MODE_PROP_ENUM) != !!dst[j].enum_values_cnt) { drmModeFreeProperty(prop); return false; }

    for (k = 0; k < dst[j].enum_values_cnt; k++) {
      int l;

      for (l = 0; l < prop->count_enums; l++) {
        if (!strcmp(prop->enums[l].name, dst[j].enum_values[k].name))
          break;
      }

      if (l == prop->count_enums) continue;

      dst[j].enum_values[k].valid = true;
      dst[j].enum_values[k].value = prop->enums[l].value;
    }

    drmModeFreeProperty(prop); prop = NULL;
  }
  
  return true;
}

/**
* Get the current value of a KMS property
*
* Given a drmModeObjectGetProperties return, as well as the drm_property_info
* for the target property, return the current value of that property,
* with an optional default. If the property is a KMS enum type, the return
* value will be translated into the appropriate internal enum.
*
* If the property is not present, the default value will be returned.
*
* @param info Internal structure for property to look up
* @param props Raw KMS properties for the target object
* @param def Value to return if property is not found
*/
static uint64_t drm_property_get_value(
  struct drm_prop_info *info,
  const drmModeObjectProperties *props,
  uint64_t def
) {

  unsigned int i;

  if (info->prop_id == 0) return def;

  for (i = 0; i < props->count_props; i++) {
    unsigned int j;

    if (props->props[i] != info->prop_id) continue;

    /* Simple (non-enum) types can return the value directly */
    if (info->enum_values_cnt == 0) return props->prop_values[i];

    /* Map from raw value to enum value */
    for (j = 0; j < info->enum_values_cnt; j++) {
      if (!info->enum_values[j].valid)
        continue;
      if (info->enum_values[j].value != props->prop_values[i])
        continue;
      return j;
    }

    /* We don't have a mapping for this enum; return default. */
    break;
  }

  return def;
}

static void plane_formats_populate(dlu_disp_core *core, uint32_t odb, drmModeObjectProperties *props) {
  uint32_t blob_id = 0;
  drmModePropertyBlobRes *blob = NULL;
  struct drm_format_modifier_blob *fmt_mod_blob = NULL; /* IN_FORMATS content */
  uint32_t *blob_formats = NULL; /* array of formats */
  struct drm_format_modifier *blob_modifiers = NULL;

  blob_id = drm_property_get_value(&core->output_data[odb].props.plane[DLU_DISPLAY_PLANE_IN_FORMATS], props, 0);
  if (blob_id == 0) {
    dlu_log_me(DLU_WARNING, "'%s' plane does not have IN_FORMATS", core->output_data[odb].name);
    return;
  }

  blob = drmModeGetPropertyBlob(core->device.kmsfd, blob_id);
  if (!blob) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPropertyBlob: %s", strerror(errno));
    return;
  }

  fmt_mod_blob = blob->data;
	
  /**
  * The IN_FORMATS blob has two variable-length arrays at the end; one of
  * uint32_t formats, and another of the supported modifiers. To allow the
  * blob to be extended and carry more information, they carry byte offsets
  * pointing to the start of the two arrays.
  */
  blob_formats = (uint32_t *) (((char *) fmt_mod_blob) + fmt_mod_blob->formats_offset);
  blob_modifiers = (struct drm_format_modifier *) (((char *) fmt_mod_blob) + fmt_mod_blob->modifiers_offset);

  for (unsigned f = 0; f < fmt_mod_blob->count_formats; f++) {
    if (blob_formats[f] != DRM_FORMAT_XRGB8888) continue;

    for (unsigned m = 0; m < fmt_mod_blob->count_modifiers; m++) {
      struct drm_format_modifier *mod = &blob_modifiers[m];

      if ((f < mod->offset) || (f > mod->offset + 63)) continue;
      if (!(mod->formats & (1 << (f - mod->offset)))) continue;
    
      core->output_data[odb].modifiers = realloc(core->output_data[odb].modifiers,
          (core->output_data[odb].modifiers_cnt + 1) * sizeof(uint32_t));

      if (!core->output_data[odb].modifiers) {
        dlu_log_me(DLU_DANGER, "[x] realloc: %s", strerror(errno));
        return;
      }

      core->output_data[odb].modifiers[core->output_data[odb].modifiers_cnt++] = mod->modifier;
    }
  }

  drmModeFreePropertyBlob(blob);
}

static void output_get_edid(dlu_disp_core *core, uint32_t odb, drmModeObjectPropertiesPtr props) {
  drmModePropertyBlobPtr blob = NULL;
  struct edid_info *edid = NULL;
  uint32_t blob_id = 0;

  blob_id = drm_property_get_value(&core->output_data[odb].props.conn[DLU_DISPLAY_CONNECTOR_EDID], props, 0);
  if (blob_id == 0) {
    dlu_log_me(DLU_WARNING, "'%s' output does not have EDID", core->output_data[odb].name);
    return;
  }

  blob = drmModeGetPropertyBlob(core->device.kmsfd, blob_id);
  if (!blob) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPropertyBlob: %s", strerror(errno));
    return;
  }

  edid = edid_parse(blob->data, blob->length);
  drmModeFreePropertyBlob(blob);
  if (!edid) return;

  dlu_log_me(DLU_INFO, "For Output Device '%s'", core->output_data[odb].name);
  dlu_log_me(DLU_INFO, "EDID PNP ID: %s", edid->pnp_id);
  dlu_log_me(DLU_INFO, "EISA ID: %s", edid->eisa_id);
  dlu_log_me(DLU_INFO, "Monitor Name: %s", edid->monitor_name);
  dlu_log_me(DLU_INFO, "Serial: %s", edid->serial_number);

  free(edid);
}

/** 
* Might be a good idea to run lucur --display-info=/dev/dri/card0 from within a TTY :)  
* or make a call to dlu_disp_q_ouput_dev_info(3).
*/
bool dlu_kms_enum_device(
  dlu_disp_core *core,
  uint32_t odb,
  uint32_t conn_idx,
  uint32_t enc_idx,
  uint32_t crtc_idx,
  uint32_t plane_idx,
  uint64_t refresh,
  const char *conn_name
) {

  bool ret = true;

  if (core->device.kmsfd == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] There appears to be no available DRM device");
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_kms_node_create()");
    goto exit_func;
  }

  if (!core->output_data) {
    PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_DATA");
    goto exit_func;
  }

  drmModeRes *dmr = drmModeGetResources(core->device.kmsfd);
  if (!dmr) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetResources: %s", strerror(errno));
    ret = false; goto exit_func;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(core->device.kmsfd);
  if (!pres) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPlaneResources: %s", strerror(errno));
    ret = false; goto exit_free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_log_me(DLU_DANGER, "[x] DRM device found is not a KMS node");
    ret = false; goto exit_free_plane_res;
  }

  core->output_data[odb].conn = drmModeGetConnector(core->device.kmsfd, dmr->connectors[conn_idx]);
  if (!core->output_data[odb].conn) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetConnector: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->output_data[odb].enc = drmModeGetEncoder(core->device.kmsfd, dmr->encoders[enc_idx]);
  if (!core->output_data[odb].enc) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetEncoder: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->output_data[odb].crtc = drmModeGetCrtc(core->device.kmsfd, dmr->crtcs[crtc_idx]);
  if (!core->output_data[odb].crtc) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetCrtc: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  core->output_data[odb].plane = drmModeGetPlane(core->device.kmsfd, pres->planes[plane_idx]);
  if (!core->output_data[odb].plane) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPlane: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  snprintf(core->output_data[odb].name, sizeof(core->output_data[odb].name), "%s", conn_name);
  core->output_data[odb].refresh = millihz_to_nsec(refresh);

  /* This members are redundant and mainly for easy of access */
  core->output_data[odb].conn_id = core->output_data[odb].conn->connector_id;
  core->output_data[odb].enc_id  = core->output_data[odb].enc->encoder_id;
  core->output_data[odb].crtc_id = core->output_data[odb].crtc->crtc_id;
  core->output_data[odb].pp_id   = core->output_data[odb].plane->plane_id;
  core->output_data[odb].mode    = core->output_data[odb].crtc->mode;

  /**
  * Now creating MODE_ID blob. When performing atomic commits, the driver expects a CRTC property with the name "MODE_ID".
  * With its name pointing to the blob id. Which is set and stored here.
  * Go here for more information: https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
  */
  if (drmModeCreatePropertyBlob(core->device.kmsfd, &core->output_data[odb].mode, sizeof(drmModeModeInfo), &core->output_data[odb].mode_blob_id) < 0) {
    dlu_log_me(DLU_DANGER, "[x] drmModeCreatePropertyBlob: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  /**
  * Objects are now created and can now get their property lists from
  * KMS and use that to fill in the props structures so we can more easily query and set them.
  * Basically store certain objects (plane, crtc, connector) to be later used in atomic modesetting.
  */
  
  /* retrieve plane properties from the KMS node */
  drmModeObjectProperties *props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].pp_id, DRM_MODE_OBJECT_PLANE);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, plane_props, core->output_data[odb].props.plane, DLU_DISPLAY_PLANE__CNT, props)) {
    dlu_log_me(DLU_DANGER, "[x] drm_prop_info_populate failed");
    ret = false; goto exit_free_mode_obj;
  }

  plane_formats_populate(core, odb, props);
  drmModeFreeObjectProperties(props); props = NULL;
  /* Plane Query */

 /* retrieve CRTC properties from the KMS node */
  props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].crtc_id, DRM_MODE_OBJECT_CRTC);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, crtc_props, core->output_data[odb].props.crtc, DLU_DISPLAY_CRTC__CNT, props)) {
    dlu_log_me(DLU_DANGER, "[x] drm_prop_info_populate failed");
    ret = false; goto exit_free_mode_obj;
  }

  drmModeFreeObjectProperties(props); props = NULL;
  /* CRTC Query */

  /* retrieve connector properties from the KMS node */
  props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].conn_id, DRM_MODE_OBJECT_CONNECTOR);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, conn_props, core->output_data[odb].props.conn, DLU_DISPLAY_CONNECTOR__CNT, props)) {
    dlu_log_me(DLU_DANGER, "[x] drm_prop_info_populate failed");
    ret = false; goto exit_free_mode_obj;
  }

  output_get_edid(core, odb, props);
  drmModeFreeObjectProperties(props); props = NULL;
  /* Connector Query */

  dlu_log_me(DLU_SUCCESS, "Enumeration successful");

exit_free_mode_obj:
  drmModeFreeObjectProperties(props);
exit_free_plane_res:
  drmModeFreePlaneResources(pres);
exit_free_drm_res:
  drmModeFreeResources(dmr);
exit_func:
  return ret;
}

/* Function name, parameters, logic may change */
bool dlu_kms_modeset(dlu_disp_core *core, uint32_t cur_bi) {

  if (core->buff_data[cur_bi].odid == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_disp_create_fb(3). Before mode setting is possible");
    return false;
  }

  /* Connecting a framebuffer to a Plane -> **"CRTC"** -> Encoder -> **"Connector"** pair. Perform actual modesetting */
  if (drmModeSetCrtc(core->device.kmsfd, core->output_data[core->buff_data[cur_bi].odid].crtc_id, core->buff_data[cur_bi].fb_id, 0, 0,
                     &core->output_data[core->buff_data[cur_bi].odid].conn_id, 1, &core->output_data[core->buff_data[cur_bi].odid].mode)) {
    dlu_log_me(DLU_DANGER, "[x] drmModeSetCrtc: %s", strerror(errno));
    return false;
  }

  return true;
}

bool dlu_kms_page_flip(dlu_disp_core *core, uint32_t cur_bi, void *user_data) {

  /* Schedules a buffer flip for the next vblank. Fully asynchronous */
  if (drmModePageFlip(core->device.kmsfd, core->output_data[core->buff_data[cur_bi].odid].crtc_id,
                      core->buff_data[cur_bi].fb_id, DRM_MODE_PAGE_FLIP_EVENT, user_data)) {
    dlu_log_me(DLU_DANGER, "[x] drmModePageFlip: %s", strerror(errno));
    return false;
  }

  core->output_data[core->buff_data[cur_bi].odid].pflip = true;

  return true;
}

int dlu_kms_handle_event(int fd, drmEventContext *ev) {
  return drmHandleEvent(fd, ev);
}

/* Set property of a connector inside of a atomic request */
static int add_conn_prop(dlu_disp_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_disp_connector_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.conn[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].conn_id, core->output_data[cur_od].props.conn[prop].prop_id, val);

  return (ret <= 0) ? -1 : 0;
}

/* Set property of a CRTC inside of a atomic request */
static int add_crtc_prop(dlu_disp_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_disp_crtc_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.crtc[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].crtc_id, core->output_data[cur_od].props.crtc[prop].prop_id, val);

  return (ret <= 0) ? -1 : 0;
}

/* Set property of a plane inside of a atomic request */
static int add_plane_prop(dlu_disp_core *core, uint32_t cur_od, drmModeAtomicReq *req, dlu_disp_plane_props prop, uint64_t val) {
  int ret = -1;

  if (!core->output_data[cur_od].props.plane[prop].prop_id) return ret;

  ret = drmModeAtomicAddProperty(req, core->output_data[cur_od].pp_id, core->output_data[cur_od].props.plane[prop].prop_id, val);

  return (ret <= 0) ? -1 : 0;
}

bool dlu_kms_atomic_req(dlu_disp_core *core, uint32_t cur_bd, drmModeAtomicReq *req) {
  int ret;

  /* Output device */
  uint32_t cur_od = core->buff_data[cur_bd].odid;
  uint32_t width = core->output_data[cur_od].mode.hdisplay;
  uint32_t height = core->output_data[cur_od].mode.vdisplay;

  // dlu_log_me(DLU_WARNING, "[%s] atomic state for commit:", core->output_data[cur_od].name);

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_CRTC_ID, core->output_data[cur_od].crtc_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_FB_ID, core->buff_data[cur_bd].fb_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_SRC_X, core->output_data[cur_od].plane->x);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_SRC_Y, core->output_data[cur_od].plane->y);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_SRC_W, width << 16);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_SRC_H, height << 16);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_CRTC_X, core->output_data[cur_od].crtc->x);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_CRTC_Y, core->output_data[cur_od].crtc->y);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_CRTC_W, width);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  ret = add_plane_prop(core, cur_od, req, DLU_DISPLAY_PLANE_CRTC_H, height);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_plane_prop: %s", strerror(errno)); return false; }

  /**
  * Changing any of these two properties requires the ALLOW_MODESET
  * flag to be set on the atomic commit.
  */
  ret = add_crtc_prop(core, cur_od, req, DLU_DISPLAY_CRTC_MODE_ID, core->output_data[cur_od].mode_blob_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_crtc_prop: %s", strerror(errno)); return false; }

  ret = add_crtc_prop(core, cur_od, req, DLU_DISPLAY_CRTC_ACTIVE, 1);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_crtc_prop: %s", strerror(errno)); return false; }

  ret = add_conn_prop(core, cur_od, req, DLU_DISPLAY_CONNECTOR_CRTC_ID, core->output_data[cur_od].crtc_id);
  if (ret == NEG_ONE) { dlu_log_me(DLU_DANGER, "[x] add_conn_prop: %s", strerror(errno)); return false; }

  return true;
}

bool dlu_kms_atomic_commit(dlu_disp_core *core, uint32_t cur_bd, drmModeAtomicReq *req, bool allow_modeset) {
  uint32_t flags = DRM_MODE_ATOMIC_NONBLOCK | DRM_MODE_PAGE_FLIP_EVENT;
  
  if (allow_modeset) /* If not set still works fine */
    flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;

  if (drmModeAtomicCommit(core->device.kmsfd, req, flags, core)) {
    dlu_log_me(DLU_DANGER, "[x] Atomic commit failed!!");
    return false;
  }

  core->output_data[core->buff_data[cur_bd].odid].pflip = true;

  return true;
}

drmModeAtomicReq *dlu_kms_atomic_alloc() { return drmModeAtomicAlloc(); }
void dlu_kms_atomic_free(drmModeAtomicReq *req) { drmModeAtomicFree(req); }

/* Open a single DRM device. Check to see if the node is a good candidate for usage */
static bool check_if_good_candidate(dlu_disp_core *core, const char *device_name) {
  drm_magic_t magic;
  uint64_t cap = 0;
  int err = 0;

  /**
  * Open the device and ensure we have support for universal planes and
  * kms atomic modesetting. This function updates the kmsfd struct member
  */
  core->device.kmsfd = logind_take_device(core, device_name);
  if (core->device.kmsfd == UINT32_MAX) goto close_kms;

  /**
  * In order to drive KMS, we need to be 'master'. This should already
  * have happened for us thanks to being root and the first client.
  * There can only be one master at a time, so this will fail if
  * (e.g.) trying to run this test whilst a graphical session is
  * already active on the current VT.
  * For more explanation:
  * https://en.wikipedia.org/wiki/Direct_Rendering_Manager#DRM-Master_and_DRM-Auth
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
  if (err || !cap)
    dlu_log_me(DLU_WARNING, "KSM node '%s' doesn't support framebuffer modifiers", device_name);
  else
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' supports framebuffer modifiers", device_name);

  cap = 0;
  err = drmGetCap(core->device.kmsfd, DRM_CAP_TIMESTAMP_MONOTONIC, &cap);
  if (err || !cap)
    dlu_log_me(DLU_WARNING, "KMS node '%s' doesn't support clock monotonic timestamps", device_name);
  else
    dlu_log_me(DLU_SUCCESS, "KMS node '%s' supports monotonic clock", device_name);

  return true;

close_kms:
  logind_release_device(core->device.kmsfd, core);
  return false;
}

void dlu_kms_vt_reset(dlu_disp_core *core) {
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
bool dlu_kms_vt_create(dlu_disp_core *core) {
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

bool dlu_kms_node_create(dlu_disp_core *core, const char *preferred_dev) {
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

static void free_drm_objs(drmModeConnector **conn, drmModeEncoder **enc, drmModeCrtc **crtc, drmModePlane **plane) {
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

  if (plane) {
    if (*plane) {
      drmModeFreePlane(*plane);
      *plane = NULL;
    }
  }
}

void dlu_print_dconf_info(const char *device) {
  dlu_otma_mems ma = { .drmc_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) return;

  dlu_disp_core *core = dlu_disp_init_core();
  if (!core) goto exit_info;

  /* Exit if not in a tty */
  if (!dlu_session_create(core)) { dlu_log_me(DLU_WARNING, "Please run command from within a TTY"); goto exit_info; }
  if (!dlu_kms_node_create(core, device)) { dlu_log_me(DLU_WARNING, "Please run command from within a TTY"); goto exit_info; }

  drmModeRes *dmr = drmModeGetResources(core->device.kmsfd);
  if (!dmr) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetResources: %s\n", strerror(errno));
    goto exit_info;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(core->device.kmsfd);
  if (!pres) {
    dlu_print_msg(DLU_DANGER, "[x] drmModeGetPlaneResources: %s\n", strerror(errno));
    goto free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_print_msg(DLU_DANGER, "[x] '%s' is not a KMS node\n", device);
    goto free_plane_res;
  }

  drmModeConnector *conn = NULL; drmModeEncoder *enc = NULL;
  drmModeCrtc *crtc = NULL; drmModePlane *plane = NULL;
  uint32_t enc_crtc_id = 0, crtc_id = 0, fb_id = 0;
  uint64_t refresh = 0;

  for (int i = 0; i < dmr->count_connectors; i++) {
    conn = drmModeGetConnector(core->device.kmsfd, dmr->connectors[i]);
    if (!conn) {
      dlu_print_msg(DLU_DANGER, "[x] drmModeGetConnector: %s\n", strerror(errno));
      free_drm_objs(&conn, &enc, &crtc, &plane);
      goto free_plane_res;
    }

    /* Finding a encoder (a deprecated KMS object) for a given connector */
    for (int e = 0; e < dmr->count_encoders; e++) {
      if (dmr->encoders[e] == conn->encoder_id) {
        enc = drmModeGetEncoder(core->device.kmsfd, dmr->encoders[e]);
        if (!enc) {
          dlu_print_msg(DLU_DANGER, "[x] drmModeGetEncoder: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          goto free_plane_res;
        }

        dlu_print_msg(DLU_SUCCESS, "\n\t\tConnector INFO\n");
        dlu_print_msg(DLU_INFO, "\tConn ID   : %u\tConn Index : %u\n", conn->connector_id, i);
        dlu_print_msg(DLU_INFO, "\tConn Type : %u\tConn Name  : %s\n", conn->connector_type, conn_types(conn->connector_type_id));
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
          goto free_plane_res;
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
          goto free_plane_res;
        }

        /* look for primary plane for chosen crtc */
        if (plane->crtc_id == crtc_id && plane->fb_id == fb_id) {
          dlu_print_msg(DLU_SUCCESS, "\n\t\tPlane INFO\n");
          dlu_print_msg(DLU_INFO, "\tPLANE ID : %u\tPlane Index : %u\n", plane->plane_id, p);
          dlu_print_msg(DLU_INFO, "\tFB ID    : %u\tCRTC ID     : %u\n", plane->fb_id, plane->crtc_id);
          dlu_print_msg(DLU_INFO, "\tgamma sz : %u\tformats     : [", plane->gamma_size);
          for (uint32_t j = 0; j < plane->count_formats; j++)
            dlu_print_msg(DLU_INFO, "%u ", plane->formats[j]);
          dlu_print_msg(DLU_INFO, "]\n");
          dlu_print_msg(DLU_DANGER, "\n\tscreen refresh: %u\n", refresh); refresh = UINT64_MAX;
          dlu_print_msg(DLU_WARNING, "\n  Plane -> CRTC -> Encoder -> Connector Pair: %d\n", (i+1));
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
  dlu_disp_freeup_core(core);
  dlu_release_blocks();
}

bool dlu_kms_q_output_chain(dlu_disp_core *core, dlu_disp_device_info *info) {
  bool ret = false; uint32_t cur_info = 0;

  if (core->device.kmsfd == UINT32_MAX) goto exit_func;

  drmModeRes *dmr = drmModeGetResources(core->device.kmsfd);
  if (!dmr) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetResources: %s\n", strerror(errno));
    goto exit_func;
  }

  drmModePlaneRes *pres = drmModeGetPlaneResources(core->device.kmsfd);
  if (!pres) {
    dlu_log_me(DLU_DANGER, "[x] drmModeGetPlaneResources: %s\n", strerror(errno));
    goto free_drm_res;
  }

  if (dmr->count_crtcs <= 0 || dmr->count_connectors <= 0 || dmr->count_encoders <= 0 || pres->count_planes <= 0) {
    dlu_log_me(DLU_DANGER, "[x] Current device is somehow not a KMS node");
    goto free_plane_res;
  }

  drmModeConnector *conn = NULL; drmModeEncoder *enc = NULL;
  drmModeCrtc *crtc = NULL; drmModePlane *plane = NULL;
  uint32_t enc_crtc_id = 0, crtc_id = 0, fb_id = 0;
  uint64_t refresh = 0;

  for (int i = 0; i < dmr->count_connectors; i++) {
    conn = drmModeGetConnector(core->device.kmsfd, dmr->connectors[i]);
    if (!conn) {
      dlu_log_me(DLU_DANGER, "[x] drmModeGetConnector: %s\n", strerror(errno));
      free_drm_objs(&conn, &enc, &crtc, &plane);
      goto free_plane_res;
    }

    /* Finding a encoder (a deprecated KMS object) for a given connector */
    for (int e = 0; e < dmr->count_encoders; e++) {
      if (dmr->encoders[e] == conn->encoder_id) {
        enc = drmModeGetEncoder(core->device.kmsfd, dmr->encoders[e]);
        if (!enc) {
          dlu_log_me(DLU_DANGER, "[x] drmModeGetEncoder: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          goto free_plane_res;
        }

        info[cur_info].enc_idx = e;
        info[cur_info].conn_idx = i;
        snprintf(info[cur_info].conn_name, sizeof(info[cur_info].conn_name), "%s", conn_types(conn->connector_type_id));

        enc_crtc_id = enc->crtc_id;
        drmModeFreeEncoder(enc); enc = NULL;
        break;
      }
    }

    /* Finding a crtc for the given encoder */
    for (int c = 0; c < dmr->count_crtcs; c++) {
      if (dmr->crtcs[c] == enc_crtc_id) {
        crtc = drmModeGetCrtc(core->device.kmsfd, dmr->crtcs[c]);
        if (!crtc) {
          dlu_log_me(DLU_DANGER, "[x] drmModeGetCrtc: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          goto free_plane_res;
        }
        
        info[cur_info].crtc_idx = c;
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
          dlu_log_me(DLU_DANGER, "[x] drmModeGetPlane: %s\n", strerror(errno));
          free_drm_objs(&conn, &enc, &crtc, &plane);
          goto free_plane_res;
        }

        /* look for primary plane for chosen crtc */
        if (plane->crtc_id == crtc_id && plane->fb_id == fb_id) {
           info[cur_info].plane_idx = p; info[cur_info].refresh  = refresh;
           cur_info++; ret = true; refresh = UINT64_MAX;
        }

        drmModeFreePlane(plane); plane = NULL;
      }

      /* Reset values */
      crtc = NULL; crtc_id = fb_id = 0;
    }

    drmModeFreeConnector(conn); conn = NULL;
  }

free_plane_res:
  drmModeFreePlaneResources(pres);
free_drm_res:
  drmModeFreeResources(dmr);  
exit_func:
  return ret;
}