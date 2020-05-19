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

#define LUCUR_VKCOMP_API
#include <lucom.h>

#include "wayland/client.h" /* Leave for now */
#include "test-extras.h"

START_TEST(test_init_vulkan) {
  dlu_otma_mems ma = { .vkcomp_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  ck_assert_ptr_nonnull(app);
  FREEME(app, NULL)
} END_TEST;

START_TEST(test_set_global_layers) {
  dlu_log_me(DLU_WARNING, "SECOND TEST");

  VkResult err;

  dlu_otma_mems ma = { .vkcomp_cnt = 1, .vkval_layer_cnt = 200 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  check_err(!app, app, NULL, NULL)

  VkLayerProperties *vk_props = VK_NULL_HANDLE;
	uint32_t size = 0;

  err = dlu_set_global_layers(&vk_props, &size);
  check_err(err, app, NULL, NULL)

  if (vk_props) {
    ck_assert_ptr_nonnull(vk_props);
  } else {
    ck_assert_ptr_null(vk_props);
  }

  FREEME(app, NULL)
} END_TEST;

START_TEST(test_create_instance) {
  VkResult err;
  dlu_log_me(DLU_WARNING, "THIRD TEST");

  dlu_otma_mems ma = { .vkcomp_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  check_err(!app, app, NULL, NULL)

  err = dlu_create_instance(app, "Hello Triangle", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, NULL, NULL)

  err = dlu_set_debug_message(app);
  check_err(err, app, NULL, NULL)

  ck_assert_ptr_nonnull(app->instance);

  FREEME(app, NULL)
} END_TEST;

START_TEST(test_enumerate_device) {
  VkResult err;
  dlu_log_me(DLU_WARNING, "FOURTH TEST");

  dlu_otma_mems ma = { .vkcomp_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  check_err(!app, app, NULL, NULL)

  err = dlu_create_instance(app, "Hello Triangle", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, NULL, NULL)

  err = dlu_set_debug_message(app);
  check_err(err, app, NULL, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = dlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, NULL, NULL)

  ck_assert_ptr_nonnull(app->physical_device);

  FREEME(app, NULL)
} END_TEST;

START_TEST(test_set_logical_device) {
  VkResult err;
  dlu_log_me(DLU_WARNING, "FIFTH TEST");

  dlu_otma_mems ma = { .vkcomp_cnt = 2 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  check_err(!app, app, NULL, NULL)

  err = dlu_create_instance(app, "Hello Triangle", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, NULL, NULL)

  err = dlu_set_debug_message(app);
  check_err(err, app, NULL, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = dlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, NULL, NULL)

  err = dlu_create_queue_families(app, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, NULL, NULL)

  app->indices.present_family = app->indices.graphics_family;
  err = dlu_create_logical_device(app, &device_feats, 1, 1, enabled_validation_layers, 1, device_extensions);
  check_err(err, app, NULL, NULL)

  FREEME(app, NULL)
} END_TEST;

START_TEST(test_swap_chain_fail_no_surface) {
  VkResult err;

  dlu_log_me(DLU_WARNING, "SIXTH TEST");
  dlu_otma_mems ma = { .vkcomp_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  vkcomp *app = dlu_init_vk();
  check_err(!app, app, NULL, NULL)

  err = dlu_create_instance(app, "Hello Triangle", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, NULL, NULL)

  err = dlu_set_debug_message(app);
  check_err(err, app, NULL, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = dlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, NULL, NULL)

  err = dlu_create_queue_families(app, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, NULL, NULL)

  app->indices.present_family = app->indices.graphics_family;
  err = dlu_create_logical_device(app, &device_feats, 1, 1, enabled_validation_layers, 1, device_extensions);
  check_err(err, app, NULL, NULL)

  ck_assert_ptr_null(app->surface);

  VkSurfaceFormatKHR surface_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkExtent2D extent2D = {1920, 1080};
  VkSurfaceCapabilitiesKHR capabilities;
  err = dlu_create_swap_chain(app, 0, capabilities, surface_fmt, VK_PRESENT_MODE_MAX_ENUM_KHR, extent2D.width, extent2D.height, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  if (err) dlu_log_me(DLU_WARNING, "[x] failed to create swap chain no surface\n");

  FREEME(app, NULL)
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
