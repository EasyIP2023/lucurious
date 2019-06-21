#include <lucom.h>
#include <vlucur/vkall.h>
#include <check.h>

START_TEST(test_init_vulkan) {
  struct vkcomp *app = NULL;
  app = init_vk();

  ck_assert_ptr_nonnull(app);

  freeup_vk(app);
  app = NULL;
} END_TEST;

START_TEST(test_set_global_layers) {
  VkResult err;
  struct vkcomp *app = NULL;
  app = init_vk();

  err = set_global_layers(app);
  if (err) {
    freeup_vk(app);
    fprintf(stderr, "[x] checking for validation layer support failed");
  }

  freeup_vk(app);
  app = NULL;

} END_TEST;

Suite *vulkan_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Vulkan");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_init_vulkan);
  tcase_add_test(tc_core, test_set_global_layers);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;

  Suite *s;
  SRunner *sr;

  s = vulkan_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
