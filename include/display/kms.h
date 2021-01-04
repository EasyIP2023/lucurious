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

#ifndef DLU_DISPLAY_KMS_H
#define DLU_DISPLAY_KMS_H

/**
* Function sets up a VT/TTY (directly switch to the TTY mode) so the process
* can in be graphical mode. It also sets a process up so that it can handle 
* its own input.
*/
bool dlu_kms_vt_create(dlu_disp_core *core);
bool dlu_kms_node_create(dlu_disp_core *core, const char *preferred_dev);

/**
* Values can be computed by running the command lucur --display-info <drm device>
* or with a call to dlu_disp_q_ouput_dev_info(3)
* This function stores the state of the initial Plane -> CRTC -> Encoder -> Connector objects
* and there properites to be later used in atomic modesetting.
*/
bool dlu_kms_enum_device(
  dlu_disp_core *core,
  uint32_t cur_odb,
  uint32_t conn_id_idx,
  uint32_t enc_id_idx,
  uint32_t crtc_id_idx,
  uint32_t plane_id_idx,
  uint64_t refresh,
  const char *conn_name
);

bool dlu_kms_modeset(dlu_disp_core *core, uint32_t cur_bi);

bool dlu_kms_page_flip(dlu_disp_core *core, uint32_t cur_bi, void *user_data);

int dlu_kms_handle_event(int fd, drmEventContext *ev);

bool dlu_kms_atomic_req(dlu_disp_core *core, uint32_t cur_bd, drmModeAtomicReq *req);

bool dlu_kms_atomic_commit(dlu_disp_core *core, uint32_t cur_bd, drmModeAtomicReq *req);

drmModeAtomicReq *dlu_kms_atomic_alloc();
void dlu_kms_atomic_free(drmModeAtomicReq *req);

bool dlu_kms_q_output_chain(dlu_disp_core *core, dlu_disp_device_info *info);

#ifdef INAPI_CALLS
void dlu_kms_vt_reset(dlu_disp_core *core);
void dlu_print_dconf_info(const char *device);
#endif

#endif