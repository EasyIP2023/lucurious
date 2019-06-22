#include <lucom.h>
#include <vlucur/vkall.h>
#include <check.h>

const char *validation_extensions[] = {
  "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor",
  "VK_LAYER_LUNARG_api_dump", "VK_LAYER_GOOGLE_threading",
  "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_parameter_validation",
  "VK_LAYER_LUNARG_vktrace", "VK_LAYER_LUNARG_standard_validation",
  "VK_LAYER_GOOGLE_unique_objects", "VK_LAYER_LUNARG_assistant_layer",
  "VK_LAYER_LUNARG_screenshot", "VK_LAYER_LUNARG_device_simulation",
  "VK_LAYER_LUNARG_core_validation"
};

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
    ck_abort_msg("[x] checking and settinn validation layers failed");
  }

  ck_assert_ptr_nonnull(app->vk_layer_props);

  for (uint32_t i = 0; i < app->vk_layer_count; i++)
    ck_assert_str_eq(app->vk_layer_props[i].layerName, validation_extensions[i]);

  freeup_vk(app);
  app = NULL;

} END_TEST;

START_TEST(test_create_instance) {
  VkResult err;
  struct vkcomp *app = NULL;
  app = init_vk();

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] checking for validation layer support failed");
  }

  ck_assert_ptr_nonnull(app->instance);
  ck_assert_ptr_nonnull(app->ep_instance_props);

  freeup_vk(app);
  app = NULL;

} END_TEST;

START_TEST(test_enumerate_device) {
  VkResult err;
  struct vkcomp *app = NULL;
  app = init_vk();

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] checking for validation layer support failed");
  }

  err = enumerate_devices(app);
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] failed to find physical device");
  }

  ck_assert_ptr_nonnull(app->physical_device);

  freeup_vk(app);
  app = NULL;
} END_TEST;


START_TEST(test_set_logical_device) {
  VkResult err;
  struct vkcomp *app = NULL;
  app = init_vk();

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] checking for validation layer support failed");
  }

  err = enumerate_devices(app);
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] failed to find physical device");
  }

  err = set_logical_device(app);
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] failed to initialize logical device to physical device");
  }

  freeup_vk(app);
  app = NULL;
} END_TEST;

START_TEST(test_swap_chain_fail_no_surface) {
  VkResult err;
  struct vkcomp *app = NULL;
  app = init_vk();

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] checking for validation layer support failed");
  }

  err = enumerate_devices(app);
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] failed to find physical device");
  }

  err = set_logical_device(app);
  if (err) {
    freeup_vk(app);
    ck_abort_msg("[x] failed to initialize logical device to physical device");
  }

  ck_assert_ptr_null(app->surface);

  err = create_swap_chain(app);
  if (err && app->surface != NULL) {
    ck_abort_msg("[x] failed to create swap chain no surface");
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
  tcase_add_test(tc_core, test_create_instance);
  tcase_add_test(tc_core, test_enumerate_device);
  tcase_add_test(tc_core, test_set_logical_device);
  tcase_add_test(tc_core, test_swap_chain_fail_no_surface);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(vulkan_suite());

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
