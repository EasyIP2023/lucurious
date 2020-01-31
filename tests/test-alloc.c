/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#include <lucom.h>
#include <check.h>

START_TEST(basic_priv_alloc) {
  wlu_otma_mems ma = {
    .inta_cnt = 1, .cha_cnt = 2,
    .fla_cnt = 1, .dba_cnt = 1,
  };
  if (!wlu_otma(WLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  int *bytes = (int *) wlu_alloc(WLU_SMALL_BLOCK_PRIV, sizeof(int));
  char **b = (char **) wlu_alloc(WLU_SMALL_BLOCK_PRIV, 2 * sizeof(b));
  float *f = (float *) wlu_alloc(WLU_SMALL_BLOCK_PRIV, sizeof(float));
  float *q = (float *) wlu_alloc(WLU_SMALL_BLOCK_PRIV, sizeof(float));

  *bytes = 30;
  b[0] = "abcdegf";
  b[1] = "hijklmn";
  *f = *q = 45.78f;
  wlu_log_me(WLU_INFO, "bytes: %d", *bytes);
  wlu_log_me(WLU_INFO, "b[0]: %s", b[0]);
  wlu_log_me(WLU_INFO, "b[1]: %s", b[1]);
  wlu_log_me(WLU_INFO, "f: %0.2f", *f);
  wlu_log_me(WLU_INFO, "q: %0.2f", *q);

  wlu_print_mb(WLU_SMALL_BLOCK_PRIV);
  FREE(WLU_SMALL_BLOCK_PRIV, b);
  wlu_log_me(WLU_WARNING, "After freeing char **b alloc");
  wlu_print_mb(WLU_SMALL_BLOCK_PRIV);

  FREE(WLU_SMALL_BLOCK_PRIV, f);
  wlu_log_me(WLU_WARNING, "After freeing float *f alloc");
  wlu_print_mb(WLU_SMALL_BLOCK_PRIV);

  wlu_release_blocks();
  bytes=NULL; q=NULL;
} END_TEST;

START_TEST(basic_shared_alloc) {
  wlu_otma_mems ma = {
    .inta_cnt = 1, .cha_cnt = 2,
    .fla_cnt = 1, .dba_cnt = 1,
  };
  if (!wlu_otma(WLU_LARGE_BLOCK_SHARED, ma)) ck_abort_msg(NULL);

  int *bytes = (int *) wlu_alloc(WLU_SMALL_BLOCK_SHARED, sizeof(int));
  char **b = (char **) wlu_alloc(WLU_SMALL_BLOCK_SHARED, 2 * sizeof(b));
  float *f = (float *) wlu_alloc(WLU_SMALL_BLOCK_SHARED, sizeof(float));
  float *q = (float *) wlu_alloc(WLU_SMALL_BLOCK_SHARED, sizeof(float));

  *bytes = 30;
  b[0] = "abcdegf";
  b[1] = "hijklmn";
  *f = *q = 45.78f;
  wlu_log_me(WLU_INFO, "bytes: %d", *bytes);
  wlu_log_me(WLU_INFO, "b[0]: %s", b[0]);
  wlu_log_me(WLU_INFO, "b[1]: %s", b[1]);
  wlu_log_me(WLU_INFO, "f: %0.2f", *f);
  wlu_log_me(WLU_INFO, "q: %0.2f", *q);

  wlu_print_mb(WLU_SMALL_BLOCK_SHARED);
  FREE(WLU_SMALL_BLOCK_SHARED, b);
  wlu_log_me(WLU_WARNING, "After freeing char **b alloc");
  wlu_print_mb(WLU_SMALL_BLOCK_SHARED);

  FREE(WLU_SMALL_BLOCK_SHARED, f);
  wlu_log_me(WLU_WARNING, "After freeing float *f alloc");
  wlu_print_mb(WLU_SMALL_BLOCK_SHARED);

  wlu_release_blocks();
  bytes=NULL; q=NULL;
} END_TEST;

Suite *alloc_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Alloc");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, basic_priv_alloc);
  tcase_add_test(tc_core, basic_shared_alloc);
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
