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

#ifndef DLU_DRM_DEVICE_H
#define DLU_DRM_DEVICE_H

/**
* Values can be computed by running the command lucur --display-info <drm device>
* This function sets up the initial Plane -> CRTC -> Connector Pairs.
* This pairs may change
*/
bool dlu_drm_kms_node_enum_ouput_dev(
  dlu_drm_core *core,
  uint32_t odb,
  uint32_t conn_id_idx,
  uint32_t enc_id_idx,
  uint32_t crtc_id_idx,
  uint32_t plane_id_idx,
  uint64_t refresh,
  const char *conn_name
);

#ifdef INAPI_CALLS
const char *ouput_devices(uint32_t type);
#endif
#endif