/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/wclient/client.h>
#include <wlu/utils/errors.h>
#include <wlu/utils/log.h>
#include <wlu/shader/shade.h>
#include <wlu/vlucur/gp.h>
#include <wlu/vlucur/matrix.h>

#include <signal.h>
#include <check.h>

#include "test-extras.h"
#include "test-shade.h"

#define NUM_DESCRIPTOR_SETS 1
#define WIDTH 1920
#define HEIGHT 1080
#define DEPTH 1

void freeme(vkcomp *app, wclient *wc, wlu_shader_info *shinfo, wlu_shader_info *shinfo_two) {
  wlu_freeup_shi(shinfo);
  wlu_freeup_shi(shinfo_two);
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
    freeme(NULL, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    ck_abort_msg(NULL);
  }

  /* Signal handler for this process */
  err = wlu_watch_me(SIGSEGV, getpid());
  if (err) {
    freeme(app, wc, NULL, NULL);
    ck_abort_msg(NULL);
  }

  wlu_add_watchme_info(1, app, 1, wc, 0, NULL);

  err = wlu_set_global_layers(app);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine", 3, enabled_validation_layers, 4, instance_extensions);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_set_debug_message(app, 1);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to setup debug message");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  if (wlu_connect_client(wc)) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to connect client");
    ck_abort_msg(NULL);
  }

  /* initialize vulkan app surface */
  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to connect to vulkan surfaceKHR");
    ck_abort_msg(NULL);
  }

  err = wlu_set_queue_family(app, VK_QUEUE_GRAPHICS_BIT);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to set device queue family");
    ck_abort_msg(NULL);
  }

  err = wlu_create_logical_device(app, 3, enabled_validation_layers, 1, device_extensions);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    ck_abort_msg(NULL);
  }

  VkSurfaceCapabilitiesKHR capabilities = wlu_q_device_capabilities(app);
  if (capabilities.minImageCount == UINT32_MAX) {
    freeme(app, wc, NULL, NULL);
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
    freeme(app, wc, NULL, NULL);
    ck_abort_msg(NULL);
  }

  VkPresentModeKHR pres_mode = wlu_choose_swap_present_mode(app);
  if (pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    freeme(app, wc, NULL, NULL);
    ck_abort_msg(NULL);
  }

  VkExtent2D extent2D = { UINT32_MAX, UINT32_MAX };
  VkExtent3D extent3D = wlu_choose_3D_swap_extent(capabilities, WIDTH, HEIGHT, DEPTH);
  if (extent3D.width == UINT32_MAX) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent3D.width equals %d", extent3D.width);
    ck_abort_msg(NULL);
  }

  wlu_retrieve_device_queue(app);

  err = wlu_create_swap_chain(app, capabilities, surface_fmt, pres_mode, extent2D, extent3D);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create swap chain");
    ck_abort_msg(NULL);
  }

  err = wlu_create_img_views(app, surface_fmt.format, VK_IMAGE_VIEW_TYPE_2D);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create image views");
    ck_abort_msg(NULL);
  }

  err = wlu_create_depth_buff(app, VK_FORMAT_D16_UNORM,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_IMAGE_TYPE_2D, extent3D,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_VIEW_TYPE_2D
  );
  if (err) {
    freeme(app, wc, NULL, NULL);
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
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_uniform_buff failed");
    ck_abort_msg(NULL);
  }

  VkDescriptorSetLayoutBinding desc_set = wlu_set_desc_set(0,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkDescriptorSetLayoutCreateInfo desc_set_info = wlu_set_desc_set_info(app, 0, NUM_DESCRIPTOR_SETS, &desc_set);

  err = wlu_create_desc_set_layout(app, &desc_set_info);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_set_desc_set_info failed");
    ck_abort_msg(NULL);
  }

  /* This is where creation of the graphics pipeline begins */

  err = wlu_create_pipeline_layout(app, 0, NULL);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_pipeline_layout failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_desc_set(app, 1, 0, 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_desc_set failed");
    ck_abort_msg(NULL);
  }

  /* start of render pass creation */

  err = wlu_create_semaphores(app);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_semaphores failed");
    ck_abort_msg(NULL);
  }

  uint32_t cur_buff = 0;
  /* Acquire the swapchain image in order to set its layout */
  err = wlu_retrieve_swapchain_img(app, &cur_buff);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_retrieve_swapchain_img failed");
    ck_abort_msg(NULL);
  }

  VkAttachmentDescription attachments[2];
  /* Create render pass color attachment for swapchain images */
  attachments[0] = wlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  /* Create render pass stencil/depth attachment for depth buffer */
  attachments[1] = wlu_set_attachment_desc(app->depth.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  );

  VkAttachmentReference color_ref = wlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkAttachmentReference depth_ref = wlu_set_attachment_ref(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  VkSubpassDescription subpass = wlu_set_subpass_desc(0, NULL, 1, &color_ref, NULL, &depth_ref, 0, NULL);

  err = wlu_create_render_pass(app, 2, attachments, 1, &subpass, 0, NULL);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_render_pass failed");
    ck_abort_msg(NULL);
  }

  wlu_log_me(WLU_SUCCESS, "Successfully created the render pass!!!");
  /* End of render pass creation */

  wlu_log_me(WLU_WARNING, "Compiling the frag code to spirv shader");
  wlu_shader_info shi_frag = wlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT,
                             fragShaderText, "frag.spv", "main");
  if (!shi_frag.bytes) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  wlu_log_me(WLU_WARNING, "Compiling the vert code to spirv shader");
  wlu_shader_info shi_vert = wlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT,
                             vertShaderText, "vert.spv", "main");
  if (!shi_vert.bytes) {
    freeme(app, wc, &shi_frag, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, shi_vert.bytes, shi_vert.byte_size);
  if (!vert_shader_module) {
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }

  wlu_add_watchme_info(1, app, 0, NULL, 1, &vert_shader_module);

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, shi_frag.bytes, shi_frag.byte_size);
  if (!frag_shader_module) {
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }

  wlu_add_watchme_info(1, app, 0, NULL, 2, &frag_shader_module);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = wlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = wlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info, frag_shader_stage_info
  };

  ALL_UNUSED(shader_stages);

  wlu_freeup_shader(app, &frag_shader_module);
  wlu_freeup_shader(app, &vert_shader_module);
  freeme(app, wc, &shi_frag, &shi_vert);
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
