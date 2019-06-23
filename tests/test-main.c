#include <lucom.h>
#include <vlucur/vkall.h>
#include <wclient/client.h>
#include <check.h>

START_TEST(test_vulkan_client_create) {
  VkResult err;
  struct wclient *wc = init_wc();
  struct vkcomp *app = init_vk();

  if (connect_client(wc)) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to connect client");
  }

  err = set_global_layers(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] checking and settinn validation layers failed");
  }

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to create vulkan instance");
  }

  err = vk_connect_surfaceKHR(app, get_display(wc), get_surface(wc));
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to connect to vulkan surfaceKHR");
  }

  err = enumerate_devices(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to find physical device");
  }

  err = set_logical_device(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to initialize logical device to physical device");
  }

  err = create_swap_chain(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to create swap chain");
  }

  err = create_img_views(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to create image views");
  }

  if (run_client(wc)) {
    freeup_wc(wc);
    freeup_vk(app);
    ck_abort_msg("[x] failed to run wayland client");
  }

  freeup_wc(wc);
  freeup_vk(app);
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
