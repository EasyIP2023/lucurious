/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2021 Vincent Davis Jr.
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

#define LUCUR_DISPLAY_API
#include <lucom.h>

/**
* File that re-implements a few libdrm calls, but in a slighty different way.
*/

/* https://gitlab.freedesktop.org/mesa/drm/-/blob/master/xf86drmMode.c#L74 */
void *drmAllocCpy(char *array, int count, int entry_size) {
  char *r;

  if (!count || !array || !entry_size)
    return 0;

  r = calloc(count*entry_size, sizeof(*r));
  if (!r) {
    kg_log_me(KG_DANGER, "[x] calloc: %s\n", strerror(errno));
    return 0;
  }

  for (int i = 0; i < count; i++)
    memcpy(r+(entry_size*i), array+(entry_size*i), entry_size);

  return r;
}

void drmModeFreeProperty(drmModePropertyPtr ptr) {
  if (!ptr) return;

  free(ptr->values);
  free(ptr->enums);
  free(ptr->blob_ids);
  free(ptr);
}

/* https://gitlab.freedesktop.org/mesa/drm/-/blob/master/xf86drmMode.c#L625 */
drmModePropertyPtr drmModeGetProperty(int fd, uint32_t property_id) {
  struct drm_mode_get_property prop;
  drmModePropertyPtr r = NULL;

  memset(&prop, 0, sizeof(prop));
  prop.prop_id = property_id;

  if (ioctl(fd, DRM_IOCTL_MODE_GETPROPERTY, &prop) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_GETPROPERTY): %s", strerror(errno));
    return NULL;
  }

  if (prop.count_values)
    prop.values_ptr = VOID2U64(alloca(prop.count_values * sizeof(uint64_t)));

  if (prop.count_enum_blobs && (prop.flags & (DRM_MODE_PROP_ENUM | DRM_MODE_PROP_BITMASK)))
    prop.enum_blob_ptr = VOID2U64(alloca(prop.count_enum_blobs * sizeof(struct drm_mode_property_enum)));

  if (prop.count_enum_blobs && (prop.flags & DRM_MODE_PROP_BLOB)) {
    prop.values_ptr = VOID2U64(alloca(prop.count_enum_blobs * sizeof(uint32_t)));
    prop.enum_blob_ptr = VOID2U64(alloca(prop.count_enum_blobs * sizeof(uint32_t)));
  }

  if (ioctl(fd, DRM_IOCTL_MODE_GETPROPERTY, &prop) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_GETPROPERTY): %s", strerror(errno));
    return NULL;
  }

  if (!(r = calloc(1,sizeof(*r)))) {
    kg_log_me(KG_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  r->prop_id = prop.prop_id;
  r->count_values = prop.count_values;

  r->flags = prop.flags;
  if (prop.count_values)
    r->values = drmAllocCpy(U642VOID(prop.values_ptr), prop.count_values, sizeof(uint64_t));
  if (prop.flags & (DRM_MODE_PROP_ENUM | DRM_MODE_PROP_BITMASK)) {
    r->count_enums = prop.count_enum_blobs;
    r->enums = drmAllocCpy(U642VOID(prop.enum_blob_ptr), prop.count_enum_blobs, sizeof(struct drm_mode_property_enum));
  } else if (prop.flags & DRM_MODE_PROP_BLOB) {
    r->values = drmAllocCpy(U642VOID(prop.values_ptr), prop.count_enum_blobs, sizeof(uint32_t));
    r->blob_ids = drmAllocCpy(U642VOID(prop.enum_blob_ptr), prop.count_enum_blobs, sizeof(uint32_t));
    r->count_blobs = prop.count_enum_blobs;
  }

  if (r->count_enums && (!r->values || !r->enums || !r->blob_ids)) {
    drmModeFreeProperty(r);
    return NULL;
  }

  strncpy(r->name, prop.name, DRM_PROP_NAME_LEN);
  r->name[DRM_PROP_NAME_LEN-1] = 0;

  return r;
}

void drmModeFreeObjectProperties(drmModeObjectPropertiesPtr ptr) {
  if (!ptr) return;

  free(ptr->props);
  free(ptr->prop_values);
  free(ptr);
}

drmModeObjectPropertiesPtr drmModeObjectGetProperties(int fd, uint32_t object_id, uint32_t object_type) {
  struct drm_mode_obj_get_properties properties;
  drmModeObjectPropertiesPtr ret = NULL;
  uint32_t count;

  memset(&properties, 0, sizeof(properties));
  properties.obj_id = object_id;
  properties.obj_type = object_type;

  if (ioctl(fd, DRM_IOCTL_MODE_OBJ_GETPROPERTIES, &properties) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_OBJ_GETPROPERTIES): %s", strerror(errno));
    return NULL;
  } 

  count = properties.count_props;

  if (count) {
    properties.props_ptr = VOID2U64(alloca(count * sizeof(uint32_t)));
    properties.prop_values_ptr = VOID2U64(alloca(count * sizeof(uint64_t)));
  }

  if (ioctl(fd, DRM_IOCTL_MODE_OBJ_GETPROPERTIES, &properties) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_OBJ_GETPROPERTIES): %s", strerror(errno));
    return NULL;
  }

  if (count < properties.count_props) {
    kg_log_me(KG_DANGER, "[x] count < properties.count_props");
    return NULL;
  }

  count = properties.count_props;

  ret = calloc(1, sizeof(*ret));
  if (!ret) {
    kg_log_me(KG_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  ret->count_props = count;
  ret->props = drmAllocCpy(U642VOID(properties.props_ptr), count, sizeof(uint32_t));
  ret->prop_values = drmAllocCpy(U642VOID(properties.prop_values_ptr), count, sizeof(uint64_t));

  if (ret->count_props && (!ret->props || !ret->prop_values)) {
    drmModeFreeObjectProperties(ret); ret=NULL;
  }

  return ret;
}

void drmModeFreePropertyBlob(drmModePropertyBlobPtr ptr) {
  if (!ptr) return;

  free(ptr->data);
  free(ptr);
}

drmModePropertyBlobPtr drmModeGetPropertyBlob(int fd, uint32_t blob_id) {
  struct drm_mode_get_blob blob;
  drmModePropertyBlobPtr r;

  memset(&blob, 0, sizeof(blob));
  blob.blob_id = blob_id;

  if (ioctl(fd, DRM_IOCTL_MODE_GETPROPBLOB, &blob) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_GETPROPBLOB): %s", strerror(errno));
    return NULL;
  }

  if (blob.length)
    blob.data = VOID2U64(alloca(blob.length));

  if (ioctl(fd, DRM_IOCTL_MODE_GETPROPBLOB, &blob) == -1) {
    kg_log_me(KG_DANGER, "[x] ioctl(DRM_IOCTL_MODE_GETPROPBLOB): %s", strerror(errno));
    return NULL;
  }

  if (!(r = calloc(1, sizeof(*r)))) {
    kg_log_me(KG_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  r->id = blob.blob_id;
  r->length = blob.length;
  r->data = drmAllocCpy(U642VOID(blob.data), 1, blob.length);
  if (!r->data) {drmModeFreePropertyBlob(r);r=NULL;}

  return r;
}

/* https://gitlab.freedesktop.org/mesa/drm/-/blob/master/xf86drmMode.c#L882 */
int drmHandleEvent(int fd, drmEventContextPtr evctx) {
  char buffer[1024];
  int len, i;
  struct drm_event *e;
  struct drm_event_vblank *vblank;
  struct drm_event_crtc_sequence *seq;
  void *user_data;

  /* The DRM read semantics guarantees that we always get only
   * complete events. */

  len = read(fd, buffer, sizeof buffer);
  if (len == 0)
    return 0;
  if (len < (int)sizeof *e)
    return -1;

  i = 0;
  while (i < len) {
    e = (struct drm_event *)(buffer + i);
    switch (e->type) {
    case DRM_EVENT_VBLANK:
      if (evctx->version < 1 ||
          evctx->vblank_handler == NULL)
        break;
      vblank = (struct drm_event_vblank *) e;
      evctx->vblank_handler(fd,
                vblank->sequence,
                vblank->tv_sec,
                vblank->tv_usec,
                U642VOID (vblank->user_data));
      break;
    case DRM_EVENT_FLIP_COMPLETE:
      vblank = (struct drm_event_vblank *) e;
      user_data = U642VOID (vblank->user_data);

      if (evctx->version >= 3 && evctx->page_flip_handler2)
        evctx->page_flip_handler2(fd,
               vblank->sequence,
               vblank->tv_sec,
               vblank->tv_usec,
               vblank->crtc_id,
               user_data);
      else if (evctx->version >= 2 && evctx->page_flip_handler)
        evctx->page_flip_handler(fd,
               vblank->sequence,
               vblank->tv_sec,
               vblank->tv_usec,
               user_data);
      break;
    case DRM_EVENT_CRTC_SEQUENCE:
      seq = (struct drm_event_crtc_sequence *) e;
      if (evctx->version >= 4 && evctx->sequence_handler)
        evctx->sequence_handler(fd,
              seq->sequence,
              seq->time_ns,
              seq->user_data);
      break;
    default:
      break;
    }
    i += e->length;
  }

  return 0;
}

