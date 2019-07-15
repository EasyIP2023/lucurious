#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/wclient/client.h>
#include <wlu/utils/errors.h>
#include <wlu/utils/log.h>
#include <signal.h>
#include <check.h>

START_TEST(test_vulkan_client_create) {
  VkResult err;

  wclient *wc = wlu_init_wc();
  if (!wc) {
    wlu_log_me(WLU_DANGER, "[x] wlu_init_wc failed!!");
    ck_abort_msg(NULL);
  }

  vkcomp *app = wlu_init_vk();
  if (!app) {
    wlu_freeup_wc(wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    ck_abort_msg(NULL);
  }

  /* Signal handler for this process */
  err = wlu_watch_me(SIGSEGV, 0, getpid(), app, wc);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to set up signal_handler");
    ck_abort_msg(NULL);
  }

  if (wlu_connect_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to connect client");
    ck_abort_msg(NULL);
  }

  err = wlu_set_global_layers(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to connect to vulkan surfaceKHR");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_set_logical_device(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_create_swap_chain(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create swap chain");
    ck_abort_msg(NULL);
  }

  err = wlu_create_img_views(app, TWO_D_IMG);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create image views");
    ck_abort_msg(NULL);
  }

  if (wlu_run_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to run wayland client");
    ck_abort_msg(NULL);
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
