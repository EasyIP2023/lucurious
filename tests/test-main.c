#include <vlucur/vkall.h>
#include <wclient/client.h>
#include <check.h>

START_TEST(test_vulkan_client_create) {
  VkResult err;
  struct wclient *wc = wlu_init_wc();
  struct vkcomp *app = wlu_init_vk();

  if (wlu_connect_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to connect client");
  }

  err = wlu_set_global_layers(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] checking and settinn validation layers failed");
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to create vulkan instance");
  }

  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to connect to vulkan surfaceKHR");
  }

  err = wlu_enumerate_devices(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to find physical device");
  }

  err = wlu_set_logical_device(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to initialize logical device to physical device");
  }

  err = wlu_create_swap_chain(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to create swap chain");
  }

  err = wlu_create_img_views(app, two_d_img);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to create image views");
  }

  if (wlu_run_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    ck_abort_msg("[x] failed to run wayland client");
  }

  wlu_freeup_wc(wc);
  wlu_freeup_vk(app);
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestMain");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_vulkan_client_create);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
