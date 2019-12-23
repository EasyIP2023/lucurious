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
#include <wlu/utils/log.h>
#include <wlu/utils/mm.h>
#include <check.h>

START_TEST(basic_alloc) {
  int *bytes = (int *) wlu_alloc(4);
  char *b = (char *) wlu_alloc(8);
  float *f = (float *) wlu_alloc(4);
  float *q = (float *) wlu_alloc(4);

  ALL_UNUSED(bytes, f, q);

  // *bytes = 30;
  // b = "abcdegf";
  // *f = *q = 45.78f;
  // wlu_log_me(WLU_INFO, "bytes: %d", *bytes);
  // wlu_log_me(WLU_INFO, "b: %s", b);
  // wlu_log_me(WLU_INFO, "f: %0.2f", *f);
  // wlu_log_me(WLU_INFO, "q: %0.2f", *q);

  wlu_print_mb();
  wlu_free(b);
  wlu_log_me(WLU_WARNING, "After freeing char *b alloc");
  wlu_print_mb();

  bytes=NULL; b=NULL; f=NULL; q=NULL;
} END_TEST;


Suite *wclient_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Alloc");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, basic_alloc);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;

  Suite *s;
  SRunner *sr;

  s = wclient_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
