#include <lucom.h>
#include <wclient/client.h>
#include <check.h>

START_TEST(init_wayland_client) {
  struct wclient *wc = NULL;
  wc = wlu_init_wc();

  ck_assert_ptr_nonnull(wc);

  wlu_freeup_wc(wc);
  wc = NULL;
} END_TEST;


Suite *wclient_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("WClient");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, init_wayland_client);
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
