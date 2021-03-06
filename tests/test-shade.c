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

#include <check.h>

#define LUCUR_SPIRV_API
#include <lucom.h>

#include "test-shade.h"

START_TEST(shade_multi_error) {
  const char source[2][80] = {
    "void main() {}",
    "#version 450\n"
    "void main() { int x = MY_DEFINE; }"
  };

  dlu_log_me(DLU_WARNING, "The first example has a compilation problem. The second does not.");

  for (int i = 0; i < 2; ++i) {
    dlu_log_me(DLU_INFO, "Source is:\n---\n%s\n---\n", source[i]);
    dlu_shader_info shinfo = dlu_compile_to_spirv(0x00000000, source[i], "vert.spv", "main");
    if (!shinfo.bytes)
      dlu_log_me(DLU_DANGER, "[x] dlu_compile_to_spirv failed");
    dlu_log_me(DLU_INFO, "Bytes for file %ld - %ld", shinfo.bytes, shinfo.byte_size);
    dlu_freeup_spriv_bytes(DLU_LIB_SHADERC_SPRIV, shinfo.result);
  }
} END_TEST;

START_TEST(shade_error) {
  const char bad_shader_src[] =
    "#version 310 es\n"
    "int main() { int main_should_be_void; }\n";

  dlu_log_me(DLU_WARNING, "Compiling a bad shader:");
  dlu_log_me(DLU_INFO, "Source is:\n---\n%s\n---\n", bad_shader_src);

  dlu_shader_info shinfo = dlu_compile_to_spirv(0x00000000, bad_shader_src, "vert.spv", "main");
  if (!shinfo.bytes) {
    dlu_log_me(DLU_DANGER, "[x] dlu_compile_to_spirv failed");
    dlu_log_me(DLU_INFO, "Bytes for file shoulde be 0 - 0");
    dlu_log_me(DLU_INFO, "%ld - %ld", shinfo.bytes, shinfo.byte_size);
  }
  dlu_freeup_spriv_bytes(DLU_LIB_SHADERC_SPRIV, shinfo.result);
} END_TEST;

START_TEST(shade_frag) {
  dlu_log_me(DLU_WARNING, "Compiling a fragmentation shader");
  dlu_shader_info shinfo = dlu_compile_to_spirv(0x00000010,
                           shader_frag_src, "frag.spv", "main");
  dlu_log_me(DLU_INFO, "Source is:\n---\n%s\n---\n", shader_frag_src);
  if (!shinfo.bytes) {
    dlu_log_me(DLU_DANGER, "[x] dlu_compile_to_spirv failed");
    dlu_log_me(DLU_INFO, "Bytes for file shoulde be 0 - 0");
    dlu_log_me(DLU_INFO, "%ld - %ld", shinfo.bytes, shinfo.byte_size);
    ck_abort_msg(NULL);
  }
  dlu_log_me(DLU_SUCCESS, "Successful Compilation of fragement shader");
  dlu_freeup_spriv_bytes(DLU_LIB_SHADERC_SPRIV, shinfo.result);
} END_TEST;

Suite *shade_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Shade");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, shade_error);
  tcase_add_test(tc_core, shade_multi_error);
  tcase_add_test(tc_core, shade_frag);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;

  Suite *s;
  SRunner *sr;

  s = shade_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
