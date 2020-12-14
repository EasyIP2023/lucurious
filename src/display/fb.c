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

#define LUCUR_DISPLAY_API
#include <lucom.h>

static bool alloc_buff(dlu_disp_core *core, dlu_disp_fb_info *info, uint32_t cur_bi) {

  core->buff_data[cur_bi].odid = info->cur_odb;
  
  switch (info->type) {
    case DLU_DISPLAY_GBM_BO:
      core->buff_data[cur_bi].bo = gbm_bo_create(core->device.gbm_device,
                                                 core->output_data[info->cur_odb].mode.hdisplay,
                                                 core->output_data[info->cur_odb].mode.vdisplay,
                                                 info->format, info->bo_flags);
      break;
    case DLU_DISPLAY_GBM_BO_WITH_MODIFIERS:
      core->buff_data[cur_bi].bo = gbm_bo_create_with_modifiers(core->device.gbm_device,
                                                                core->output_data[info->cur_odb].mode.hdisplay,
                                                                core->output_data[info->cur_odb].mode.vdisplay,
                                                                info->format,core->output_data[info->cur_odb].modifiers,
                                                                core->output_data[info->cur_odb].modifiers_cnt);
      break;
    default: break;
  }

  if (!core->buff_data[cur_bi].bo) {
    dlu_log_me(DLU_DANGER, "[x] failed to create gbm_bo with res %u x %u",
                           core->output_data[info->cur_odb].mode.hdisplay,
                           core->output_data[info->cur_odb].mode.vdisplay);
    return false;
  }

  core->buff_data[cur_bi].num_planes = gbm_bo_get_plane_count(core->buff_data[cur_bi].bo);
  core->buff_data[cur_bi].modifier = gbm_bo_get_modifier(core->buff_data[cur_bi].bo);
  core->buff_data[cur_bi].format = info->format;

  for (uint32_t i = 0; i < core->buff_data[cur_bi].num_planes; i++) {
    union gbm_bo_handle h;

    h = gbm_bo_get_handle_for_plane(core->buff_data[cur_bi].bo, i);
    if (!h.u32 || h.s32 == NEG_ONE) {
      dlu_log_me(DLU_DANGER, "[x] failed to get BO plane %d gem handle (modifier 0x%" PRIx64 ")", i, core->buff_data[cur_bi].modifier);
      return false;
    }

    core->buff_data[cur_bi].gem_handles[i] = h.u32;

    core->buff_data[cur_bi].pitches[i] = gbm_bo_get_stride_for_plane(core->buff_data[cur_bi].bo, i);
    if (!core->buff_data[cur_bi].pitches[i]) {
      dlu_log_me(DLU_DANGER, "[x] failed to get stride/pitch for BO plane %d (modifier 0x%" PRIx64 ")", i, core->buff_data[cur_bi].modifier);
      return false;
    }

    core->buff_data[cur_bi].offsets[i] = gbm_bo_get_offset(core->buff_data[cur_bi].bo, i);

    struct drm_prime_handle prime_request = {
      .handle = core->buff_data[cur_bi].gem_handles[i],
      .flags  = DRM_RDWR,
      .fd     = -1
    };

    /* Retrieve a DMA-BUF fd from the GEM handle/name to pass along to other processes */
    if (ioctl(core->device.kmsfd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime_request) == NEG_ONE)  {
      dlu_log_me(DLU_DANGER, "[x] ioctl: %s", strerror(errno));
      return false;
    }

    core->buff_data[cur_bi].dma_buf_fds[i] = prime_request.fd;
  }

  /* Create actual framebuffer */
  switch (info->type) {
    case DLU_DISPLAY_GBM_BO:
      {
        /* Create actual framebuffer */
        struct drm_mode_fb_cmd f;
        memset(&f,0,sizeof(struct drm_mode_fb_cmd));
        f.bpp    = info->bpp;
        f.depth  = info->depth;
        f.width  = core->output_data[info->cur_odb].mode.hdisplay;
        f.height = core->output_data[info->cur_odb].mode.vdisplay;
        f.pitch  = core->buff_data[cur_bi].pitches[0];
        f.handle = core->buff_data[cur_bi].gem_handles[0];
        core->buff_data[cur_bi].fb_id = 0;

        if (ioctl(core->device.kmsfd, DRM_IOCTL_MODE_ADDFB, &f) == NEG_ONE) {
          dlu_log_me(DLU_DANGER, "[x] ioctl(DRM_IOCTL_MODE_ADDFB): %s", strerror(errno));
          return false;
        }

        core->buff_data[cur_bi].fb_id = f.fb_id;
      }
      break;
    case DLU_DISPLAY_GBM_BO_WITH_MODIFIERS:
      {
        struct drm_mode_fb_cmd2 f;
        memset(&f,0,sizeof(struct drm_mode_fb_cmd2));
        f.width  = core->output_data[info->cur_odb].mode.hdisplay;
        f.height = core->output_data[info->cur_odb].mode.vdisplay;
        f.pixel_format = info->format;
        f.flags = info->flags;
        
        memcpy(f.handles, core->buff_data[cur_bi].gem_handles, 4 * sizeof(uint32_t));
        memcpy(f.pitches, core->buff_data[cur_bi].pitches, 4 * sizeof(uint32_t));
        memcpy(f.offsets, core->buff_data[cur_bi].offsets, 4 * sizeof(uint32_t));
        memcpy(f.modifier, core->output_data[info->cur_odb].modifiers, 4 * sizeof(uint32_t));
        core->buff_data[cur_bi].fb_id = 0;

        if (ioctl(core->device.kmsfd, DRM_IOCTL_MODE_ADDFB2, &f) == NEG_ONE) {
          dlu_log_me(DLU_DANGER, "[x] ioctl(DRM_IOCTL_MODE_ADDFB2): %s", strerror(errno));
          return false;
        }

        core->buff_data[cur_bi].fb_id = f.fb_id;
      }
      break;
    default: break;
  }

  dlu_log_me(DLU_SUCCESS, "Successfully created gbm framebuffer");

  return true;
}

bool dlu_fb_create(
  dlu_disp_core *core,
  uint32_t bo_count,
  dlu_disp_fb_info *fb_info
) {

  if (!core->buff_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_DEVICE_OUTPUT_BUFF_DATA"); return false; }

  if (core->device.kmsfd == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] There appears to be no available DRM device");
    dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_kms_node_create()");
    return false;
  }

  core->device.gbm_device = gbm_create_device(core->device.kmsfd);
  if (!core->device.gbm_device) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a gbm device");
    return false;
  }

  for (uint32_t i=0; i < bo_count; i++)
    if (!alloc_buff(core, fb_info, i))
      return false;

  return true;
}

void *dlu_fb_gbm_bo_map(dlu_disp_core *core, uint32_t cur_bi, void **data, uint32_t flags) {
  return gbm_bo_map(core->buff_data[cur_bi].bo, 0, 0,
                    core->output_data[core->buff_data[cur_bi].odid].mode.hdisplay,
                    core->output_data[core->buff_data[cur_bi].odid].mode.vdisplay,
                    flags, &core->buff_data[cur_bi].pitches[0], data);
}

void dlu_fb_gbm_bo_unmap(struct gbm_bo *bo, void *map_data) {
  gbm_bo_unmap(bo, map_data);
}

int dlu_fb_gbm_bo_write(struct gbm_bo *bo, const void *buff, size_t count) {
  return gbm_bo_write(bo, buff, count);
}