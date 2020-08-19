/**
* This file is an exact copy of what's in kms-quads:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
*/

#define LUCUR_DRM_API
#include <lucom.h>

/* All properties and enum values supported one needs to */
static struct drm_prop_enum_info plane_type_enums[] = {
  [DLU_DRM_PLANE_TYPE_PRIMARY] = { .name = "Primary" },
  [DLU_DRM_PLANE_TYPE_OVERLAY] = { .name = "Overlay" },
  [DLU_DRM_PLANE_TYPE_CURSOR]  = { .name = "Cursor"  }
};

static const struct drm_prop_info plane_props[] = {
  [DLU_DRM_PLANE_TYPE] = {
    .name = "type",
    .enum_values = plane_type_enums,
    .enum_values_cnt = DLU_DRM_PLANE_TYPE__CNT,
  },
  [DLU_DRM_PLANE_SRC_X] = { .name = "SRC_X" },
  [DLU_DRM_PLANE_SRC_Y] = { .name = "SRC_Y" },
  [DLU_DRM_PLANE_SRC_W] = { .name = "SRC_W" },
  [DLU_DRM_PLANE_SRC_H] = { .name = "SRC_H" },
  [DLU_DRM_PLANE_CRTC_X] = { .name = "CRTC_X" },
  [DLU_DRM_PLANE_CRTC_Y] = { .name = "CRTC_Y" },
  [DLU_DRM_PLANE_CRTC_W] = { .name = "CRTC_W" },
  [DLU_DRM_PLANE_CRTC_H] = { .name = "CRTC_H" },
  [DLU_DRM_PLANE_FB_ID] = { .name = "FB_ID" },
  [DLU_DRM_PLANE_CRTC_ID] = { .name = "CRTC_ID", },
  [DLU_DRM_PLANE_IN_FORMATS] = { .name = "IN_FORMATS" },
  [DLU_DRM_PLANE_IN_FENCE_FD] = { .name = "IN_FENCE_FD" }
};

/* Display Power Management Signaling */
static struct drm_prop_enum_info dpms_state_enums[] = {
  [DLU_DRM_DPMS_STATE_OFF] = { .name = "Off" },
  [DLU_DRM_DPMS_STATE_ON] = { .name = "On" }
};

static const struct drm_prop_info conn_props[] = {
  [DLU_DRM_CONNECTOR_EDID] = { .name = "EDID" },
  [DLU_DRM_CONNECTOR_DPMS] = {
    .name = "DPMS",
    .enum_values = dpms_state_enums,
    .enum_values_cnt = DLU_DRM_DPMS_STATE__CNT,
  },
  [DLU_DRM_CONNECTOR_CRTC_ID] = { .name = "CRTC_ID" },
  [DLU_DRM_CONNECTOR_NON_DESKTOP] = { .name = "non-desktop" }
};

static const struct drm_prop_info crtc_props[] = {
  [DLU_DRM_CRTC_MODE_ID] = { .name = "MODE_ID" },
  [DLU_DRM_CRTC_ACTIVE] = { .name = "ACTIVE" },
  [DLU_DRM_CRTC_OUT_FENCE_PTR] = { .name = "OUT_FENCE_PTR" }
};

static bool drm_prop_info_populate(
  dlu_drm_core *core,
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

static void plane_formats_populate(dlu_drm_core *core, uint32_t odb, drmModeObjectProperties *props) {
  uint32_t blob_id = 0;
  drmModePropertyBlobRes *blob = NULL;
  struct drm_format_modifier_blob *fmt_mod_blob = NULL; /* IN_FORMATS content */
  uint32_t *blob_formats = NULL; /* array of formats */
  struct drm_format_modifier *blob_modifiers = NULL;

  blob_id = drm_property_get_value(&core->output_data[odb].props.plane[DLU_DRM_PLANE_IN_FORMATS], props, 0);
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

static void output_get_edid(dlu_drm_core *core, uint32_t odb, drmModeObjectPropertiesPtr props) {
  drmModePropertyBlobPtr blob = NULL;
  struct edid_info *edid = NULL;
  uint32_t blob_id = 0;

  blob_id = drm_property_get_value(&core->output_data[odb].props.conn[DLU_DRM_CONNECTOR_EDID], props, 0);
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
* or make a call to dlu_drm_q_ouput_dev_info(3).
*/
bool dlu_drm_kms_node_enum_ouput_dev(
  dlu_drm_core *core,
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
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_drm_create_kms_node()");
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
  * Now creating MODE_ID blob
  * Go here for more information: https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/kms.c
  */
  if (drmModeCreatePropertyBlob(core->device.kmsfd, &core->output_data[odb].mode, sizeof(drmModeModeInfo), &core->output_data[odb].mode_blob_id) < 0) {
    dlu_log_me(DLU_DANGER, "[x] drmModeCreatePropertyBlob: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }

  /**
  * Objects are now created and can now get their property lists from
  * KMS and use that to fill in the props structures so we can more easily
  * query and set them.
  */
  
  /* Plane Query */
  drmModeObjectProperties *props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].pp_id, DRM_MODE_OBJECT_PLANE);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, plane_props, core->output_data[odb].props.plane, DLU_DRM_PLANE__CNT, props)) {
    dlu_log_me(DLU_DANGER, "[x] drm_prop_info_populate failed");
    ret = false; goto exit_free_mode_obj;
  }

  plane_formats_populate(core, odb, props);
  drmModeFreeObjectProperties(props); props = NULL;
  /* Plane Query */

  /* CRTC Query */
  props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].crtc_id, DRM_MODE_OBJECT_CRTC);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, crtc_props, core->output_data[odb].props.crtc, DLU_DRM_CRTC__CNT, props)) {
    dlu_log_me(DLU_DANGER, "[x] drm_prop_info_populate failed");
    ret = false; goto exit_free_mode_obj;
  }

  drmModeFreeObjectProperties(props); props = NULL;
  /* CRTC Query */

  /* Connector Query */
  props = drmModeObjectGetProperties(core->device.kmsfd, core->output_data[odb].conn_id, DRM_MODE_OBJECT_CONNECTOR);
  if (!props) {
    dlu_log_me(DLU_DANGER, "[x] drmModeObjectGetProperties: %s", strerror(errno));
    ret = false; goto exit_free_plane_res;
  }
  
  if (!drm_prop_info_populate(core, conn_props, core->output_data[odb].props.conn, DLU_DRM_CONNECTOR__CNT, props)) {
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
