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

START_TEST(init_create_drm_core_struct) {
  dlu_otma_mems ma = { .drmc_cnt = 1 };

  if (!dlu_otma(DLU_LARGE_BLOCK_SHARED, ma))
    ck_abort_msg(NULL);
  
  dlu_drm_core *core = dlu_init_drm_core();
  if (!dlu_modeset_open(core, "/dev/dri/card0"))
    ck_abort_msg(NULL);

  dlu_freeup_drm_core(core);

  dlu_release_blocks();
} END_TEST;

Suite *alloc_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("DRMCoreBasics");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, init_create_drm_core_struct);
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
