#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
#include <check.h>

const char *enabled_validation_layers[] = {
  "VK_LAYER_LUNARG_core_validation", "VK_LAYER_KHRONOS_validation",
  "VK_LAYER_LUNARG_monitor", "VK_LAYER_LUNARG_api_dump",
  "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_object_tracker",
  "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_vktrace",
  "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_GOOGLE_unique_objects",
  "VK_LAYER_LUNARG_assistant_layer", "VK_LAYER_LUNARG_screenshot",
  "VK_LAYER_LUNARG_device_simulation"
};

START_TEST(test_init_vulkan) {
  vkcomp *app = NULL;
  app = wlu_init_vk();

  ck_assert_ptr_nonnull(app);

  wlu_freeup_vk(app);
  app = NULL;
} END_TEST;

START_TEST(test_set_global_layers) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_set_global_layers(app);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  ck_assert_ptr_nonnull(app->vk_layer_props);

  for (uint32_t i = 0; i < app->vk_layer_count; i++)
    ck_assert_str_eq(app->vk_layer_props[i].layerName, enabled_validation_layers[i]);

  wlu_freeup_vk(app);
  app = NULL;

} END_TEST;

START_TEST(test_create_instance) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  ck_assert_ptr_nonnull(app->instance);
  ck_assert_ptr_nonnull(app->ep_instance_props);

  wlu_freeup_vk(app);
  app = NULL;

} END_TEST;

START_TEST(test_enumerate_device) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  ck_assert_ptr_nonnull(app->physical_device);

  wlu_freeup_vk(app);
  app = NULL;
} END_TEST;


START_TEST(test_set_logical_device) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_set_logical_device(app);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    ck_abort_msg(NULL);
  }

  wlu_freeup_vk(app);
  app = NULL;
} END_TEST;

START_TEST(test_swap_chain_fail_no_surface) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_set_logical_device(app);
  if (err) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    ck_abort_msg(NULL);
  }

  ck_assert_ptr_null(app->surface);

  VkSurfaceCapabilitiesKHR capabilities = wlu_q_device_capabilities(app);
  if (capabilities.minImageCount == UINT32_MAX) {
    wlu_freeup_vk(app);
    ck_abort_msg(NULL);
  }

  VkSurfaceFormatKHR surface_fmt = wlu_choose_swap_surface_format(app, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  if (surface_fmt.format == VK_FORMAT_UNDEFINED) {
    wlu_freeup_vk(app);
    ck_abort_msg(NULL);
  }

  VkPresentModeKHR pres_mode = wlu_choose_swap_present_mode(app);
  if (pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    wlu_freeup_vk(app);
    ck_abort_msg(NULL);
  }

  VkExtent2D extent = wlu_choose_swap_extent(capabilities);
  if (extent.width == UINT32_MAX) {
    wlu_freeup_vk(app);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent.width equals %d", extent.width);
    ck_abort_msg(NULL);
  }

  err = wlu_create_swap_chain(app, capabilities, surface_fmt, pres_mode, extent);
  if (err) wlu_log_me(WLU_WARNING, "[x] failed to create swap chain no surface\n");

  wlu_freeup_vk(app);
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
