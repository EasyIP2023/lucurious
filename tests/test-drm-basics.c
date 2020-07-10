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
#include <check.h>


static void free_core(dlu_drm_core *core) {
  dlu_drm_freeup_core(core);
  dlu_release_blocks();
}

START_TEST(init_create_kms_node) {
  dlu_otma_mems ma = { .drmc_cnt = 1 };

  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma))
    ck_abort_msg(NULL);
  
  dlu_drm_core *core = dlu_drm_init_core();

  /** 
  * RUN IN TTY:
  * First creates a logind session. This allows for access to
  * privileged devices without being root.
  * Then find a suitable kms node = drm device = gpu
  */
  if (!dlu_drm_create_session(core))
    goto exit_create_kms_node; // Exit if not in a tty
 
  if (!dlu_drm_create_kms_node(core, "/dev/dri/card0")) {
    free_core(core);
    ck_abort_msg(NULL);
  }

exit_create_kms_node:
  free_core(core);
} END_TEST;

START_TEST(kms_node_enumeration_gbm_bo_creation) {
  dlu_otma_mems ma = { .drmc_cnt = 1, .dod_cnt = 1, .odb_cnt = 1 };

  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma))
    ck_abort_msg(NULL);

  dlu_drm_core *core = dlu_drm_init_core();

  if (!dlu_otba(DLU_DEVICE_OUTPUT_DATA, core, INDEX_IGNORE, 1))
    ck_abort_msg(NULL);

  if (!dlu_otba(DLU_DEVICE_OUTPUT_BUFF_DATA, core, INDEX_IGNORE, 1))
    ck_abort_msg(NULL);

  /**
  * RUN IN TTY:
  * First creates a logind session. This allows for access to
  * privileged devices without being root.
  * Then find a suitable kms node = drm device = gpu
  */
  if (!dlu_drm_create_session(core))
    goto exit_create_kms_node; // Exit if not in a tty

  if (!dlu_drm_create_kms_node(core, NULL)) {
    free_core(core);
    ck_abort_msg(NULL);
  }

  dlu_drm_device_info dinfo[1];  
  if (!dlu_drm_q_output_dev_info(core, dinfo) ) {
    free_core(core);
    ck_abort_msg(NULL);
  }

  uint32_t cur_odb = 0;
  /* Indexes for my particular system kms node */
  if (!dlu_drm_kms_node_enum_ouput_dev(core, cur_odb, dinfo->conn_idx, dinfo->enc_idx, dinfo->crtc_idx, dinfo->plane_idx, dinfo->refresh, dinfo->conn_name)) {
    free_core(core);
    ck_abort_msg(NULL);
  }

  if (!dlu_drm_create_gbm_device(core)) {
    free_core(core);
    ck_abort_msg(NULL);  
  }

  if (!dlu_drm_create_gbm_bo(DLU_DRM_GBM_BO, core, 0, cur_odb, DRM_FORMAT_XRGB8888)) {
    free_core(core);
    ck_abort_msg(NULL);
  }

exit_create_kms_node:
  free_core(core);
} END_TEST;

Suite *alloc_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("DRMCoreBasics");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, init_create_kms_node);
  tcase_add_test(tc_core, kms_node_enumeration_gbm_bo_creation);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;

  Suite *s = alloc_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
