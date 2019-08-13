#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/wclient/client.h>
#include <wlu/utils/log.h>
#include <wlu/shader/shade.h>
#include <wlu/vlucur/gp.h>
#include <wlu/vlucur/matrix.h>

#include <check.h>

#include "test-extras.h"
#include "test-shade.h"

#define NUM_DESCRIPTOR_SETS 1
#define WIDTH 1920
#define HEIGHT 1080
#define DEPTH 1

void freesh(shaderc_compiler_t compiler, shaderc_compilation_result_t result) {
  shaderc_result_release(result);
  shaderc_compiler_release(compiler);
}

void freeme(vkcomp *app, wclient *wc) {
  wlu_freeup_vk(app);
  wlu_freeup_wc(wc);
}

START_TEST(test_vulkan_client_create_3D) {
  VkResult err;

  wclient *wc = wlu_init_wc();
  if (!wc) {
    wlu_log_me(WLU_DANGER, "[x] wlu_init_wc failed!!");
    ck_abort_msg(NULL);
  }

  vkcomp *app = wlu_init_vk();
  if (!app) {
    freeme(NULL, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    ck_abort_msg(NULL);
  }

  err = wlu_set_global_layers(app);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine", 0, NULL, 3, instance_extensions);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  if (wlu_connect_client(wc)) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to connect client");
    ck_abort_msg(NULL);
  }

  /* initialize vulkan app surface */
  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to connect to vulkan surfaceKHR");
    ck_abort_msg(NULL);
  }

  err = wlu_set_queue_family(app, VK_QUEUE_GRAPHICS_BIT);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to set device queue family");
    ck_abort_msg(NULL);
  }

  err = wlu_create_logical_device(app, 0, NULL, 1, device_extensions);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    ck_abort_msg(NULL);
  }

  VkSurfaceCapabilitiesKHR capabilities = wlu_q_device_capabilities(app);
  if (capabilities.minImageCount == UINT32_MAX) {
    freeme(app, wc);
    ck_abort_msg(NULL);
  }

  /*
   * VK_FORMAT_B8G8R8A8_UNORM will store the B, G, R and alpha channels
   * in that order with an 8 bit unsigned integer and a total of 32 bits per pixel.
   * SRGB if used for colorSpace if available, because it
   * results in more accurate perceived colors
   */
  VkSurfaceFormatKHR surface_fmt = wlu_choose_swap_surface_format(app, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  if (surface_fmt.format == VK_FORMAT_UNDEFINED) {
    freeme(app, wc);
    ck_abort_msg(NULL);
  }

  VkPresentModeKHR pres_mode = wlu_choose_swap_present_mode(app);
  if (pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    freeme(app, wc);
    ck_abort_msg(NULL);
  }

  VkExtent3D extent = wlu_choose_3D_swap_extent(capabilities, WIDTH, HEIGHT, DEPTH);
  if (extent.width == UINT32_MAX) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent.width equals %d", extent.width);
    ck_abort_msg(NULL);
  }

  err = wlu_create_depth_buff(app, VK_FORMAT_D16_UNORM,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_IMAGE_TYPE_2D, extent,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_VIEW_TYPE_2D
  );
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_depth_buff failed");
    ck_abort_msg(NULL);
  }

  wlu_set_perspective(app, 45.0f, 1.0f, 0.1f, 100.0f);

  float dir[3] = {-5, 3, -10};
  float eye[3] = {0, 0, 0};
  float up[3] = {0, -1, 0};
  wlu_set_lookat(app, dir, eye, up);

  wlu_set_model_matrix(app, 1.0f);

  float clip_matrix[4][4] = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f,-1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.5f, 0.0f },
    { 0.0f, 0.0f, 0.5f, 1.0f }
  };
  wlu_set_clip_matrix(app, clip_matrix);
  wlu_set_mvp_matrix(app);
  wlu_print_matrices(app);

  err = wlu_create_uniform_buff(app, 0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_uniform_buff failed");
    ck_abort_msg(NULL);
  }

  VkDescriptorSetLayoutBinding desc_set = wlu_set_desc_set(0,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkDescriptorSetLayoutCreateInfo desc_set_info = wlu_set_desc_set_info(app, 0, NUM_DESCRIPTOR_SETS, &desc_set);

  err = wlu_create_desc_set(app, &desc_set_info);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_set_desc_set_info failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_pipeline_layout(app, 0, NULL);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_pipeline_layout failed");
    ck_abort_msg(NULL);
  }

  // wlu_freeup_shader(app, frag_shader_module);
  // wlu_freeup_shader(app, vert_shader_module);
  // freesh(compiler, result);
  freeme(app, wc);
} END_TEST;


Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("Test3DMain");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_vulkan_client_create_3D);
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
