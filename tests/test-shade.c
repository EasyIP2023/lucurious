#include <stdlib.h>
#include <wlu/shader/shade.h>
#include <wlu/utils/log.h>
#include <check.h>
#include <stdbool.h>

START_TEST(shade_multi_error) {
  const char source[2][80] = {
    "void main() {}",
    "#version 450\n"
    "void main() { int x = MY_DEFINE; }"
  };

  wlu_log_me(WLU_WARNING, "The first example has a compilation problem. The second does not.");

  shaderc_compiler_t compiler = shaderc_compiler_initialize();

  for (int i = 0; i < 2; ++i) {
    wlu_log_me(WLU_INFO, "Source is:\n---\n%s\n---\n", source[i]);
    shaderc_compilation_result_t result = 0;
    wlu_shader_info shinfo = wlu_compile_to_spirv(compiler, result,
                             shaderc_glsl_vertex_shader, source[i],
                             "main.vert", "main", true);
    if (!shinfo.bytes)
      wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    shaderc_result_release(result);
  }
  shaderc_compiler_release(compiler);
} END_TEST;

START_TEST(shade_error) {
  const char bad_shader_src[] =
    "#version 310 es\n"
    "int main() { int main_should_be_void; }\n";

  shaderc_compiler_t compiler = shaderc_compiler_initialize();
  shaderc_compilation_result_t result = 0;

  wlu_log_me(WLU_WARNING, "Compiling a bad shader:");
  wlu_shader_info shinfo = wlu_compile_to_spirv(compiler, result,
                           shaderc_glsl_vertex_shader, bad_shader_src,
                           "bad_src", "main", false);
  if (!shinfo.bytes)
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");

  shaderc_result_release(result);
  shaderc_compiler_release(compiler);
} END_TEST;


Suite *shade_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Shade");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, shade_error);
  tcase_add_test(tc_core, shade_multi_error);
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
