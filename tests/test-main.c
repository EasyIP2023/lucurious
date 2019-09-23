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

#define WIDTH 800
#define HEIGHT 600

void freeme(vkcomp *app, wclient *wc) {
  wlu_freeup_vk(app);
  wlu_freeup_wc(wc);
  wlu_freeup_watchme();
}

START_TEST(test_vulkan_client_create) {
  VkResult err;

  wclient *wc = wlu_init_wc();
  if (!wc) {
    wlu_log_me(WLU_DANGER, "[x] wlu_init_wc failed!!");
    ck_abort_msg(NULL);
  }

  vkcomp *app = wlu_init_vk();
  if (!app) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    ck_abort_msg(NULL);
  }

  /* Signal handler for this process */
  err = wlu_watch_me(SIGSEGV, getpid());
  if (err) {
    freeme(app, wc);
    ck_abort_msg(NULL);
  }

  wlu_add_watchme_info(1, app, 1, wc, 0, NULL);

  err = wlu_set_global_layers(app);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine", 3, enabled_validation_layers, 4, instance_extensions);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_set_debug_message(app, 1);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to setup debug message");
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

  err = wlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_set_queue_family(app, VK_QUEUE_GRAPHICS_BIT);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to set device queue family");
    ck_abort_msg(NULL);
  }

  err = wlu_create_logical_device(app, 3, enabled_validation_layers, 1, device_extensions);
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

  VkExtent2D extent2D = wlu_choose_2D_swap_extent(capabilities, WIDTH, HEIGHT);
  if (extent2D.width == UINT32_MAX) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent2D.width equals %d", extent2D.width);
    ck_abort_msg(NULL);
  }

  err = wlu_create_swap_chain(app, capabilities, surface_fmt, pres_mode, extent2D.width, extent2D.height);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create swap chain");
    ck_abort_msg(NULL);
  }

  err = wlu_create_cmd_pool(app, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create command pool, ERROR CODE: %d", err);
    ck_abort_msg(NULL);
  }

  err = wlu_create_cmd_buffs(app, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create command buffers, ERROR CODE: %d", err);
    ck_abort_msg(NULL);
  }

  err = wlu_exec_begin_cmd_buffs(app, 0, NULL);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to start command buffer recording");
    ck_abort_msg(NULL);
  }

  err = wlu_create_img_views(app, surface_fmt.format, VK_IMAGE_VIEW_TYPE_2D);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create image views");
    ck_abort_msg(NULL);
  }

  /* This is where creation of the graphics pipeline begins */
  err = wlu_create_semaphores(app);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_semaphores failed");
    ck_abort_msg(NULL);
  }

  uint32_t cur_buff = 0;
  /* Acquire the swapchain image in order to set its layout */
  err = wlu_retrieve_swapchain_img(app, &cur_buff);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_retrieve_swapchain_img failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_pipeline_layout(app, 0, NULL);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_pipeline_layout failed");
    ck_abort_msg(NULL);
  }

  /* Starting point for render pass creation */
  VkAttachmentDescription color_attachment = wlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  VkAttachmentReference color_attachment_ref = wlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  VkSubpassDescription subpass = wlu_set_subpass_desc(
    0, NULL, 1, &color_attachment_ref, NULL, NULL, 0, NULL
  );

  VkSubpassDependency subdep = wlu_set_subpass_dep(
    VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0
  );

  err = wlu_create_render_pass(app, 1, &color_attachment, 1, &subpass, 1, &subdep);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create render pass");
    ck_abort_msg(NULL);
  }
  wlu_log_me(WLU_SUCCESS, "Successfully created render pass");
  /* ending point for render pass creation */

  wlu_log_me(WLU_WARNING, "Compiling the frag code to spirv shader");

  wlu_shader_info shi_frag = wlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT,
                             shader_frag_src, "frag.spv", "main");
  if (!shi_frag.bytes) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  wlu_log_me(WLU_WARNING, "Compiling the vert code to spirv shader");
  wlu_shader_info shi_vert = wlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT,
                             shader_vert_src, "vert.spv", "main");
  if (!shi_vert.bytes) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  VkImageView vkimg_attach[1];
  err = wlu_create_framebuffers(app, 1, vkimg_attach, extent2D.width, extent2D.height, 1);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_framebuffers failed");
    ck_abort_msg(NULL);
  }

  /* Start of vertex buffer */
  vertex_2D vertices[3];
  for (uint32_t i = 0; i < 3; i++) {
    wlu_set_vector(&vertices[i].pos, pos_vertices[i], WLU_VEC2);
    wlu_set_vector(&vertices[i].color, color_vertices[i], WLU_VEC3);
    wlu_print_vector(&vertices[i].pos, WLU_VEC2);
    wlu_print_vector(&vertices[i].color, WLU_VEC3);
  }

  VkDeviceSize vsize = sizeof(vertices[0]) * 3;
  // err = wlu_create_buffer(
  //   app, vsize, vertices, WLU_VERTEX_2D, 0,
  //   VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "staging",
  //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  // );
  // if (err) {
  //   freeme(app, wc);
  //   wlu_log_me(WLU_DANGER, "[x] wlu_create_uniform_buff failed");
  //   ck_abort_msg(NULL);
  // }

  /*
   * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
   * The VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT communicates that the memory
   * should be mapped so that the CPU (host) can access it.
   * The VK_MEMORY_PROPERTY_HOST_COHERENT_BIT requests that the
   * writes to the memory by the host are visible to the device
   * (and vice-versa) without the need to flush memory caches.
   */
  err = wlu_create_buffer(
    app, vsize, vertices, WLU_VERTEX_2D, 0,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, "vertex",
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_uniform_buff failed");
    ck_abort_msg(NULL);
  }

  // err = wlu_copy_buffer(app, app->buffs_data[0].buff, app->buffs_data[1].buff, vsize);
  // if (err) {
  //   freeme(app, wc);
  //   wlu_log_me(WLU_DANGER, "[x] wlu_copy_buffer failed");
  //   ck_abort_msg(NULL);
  // }

  VkVertexInputBindingDescription vi_binding = wlu_set_vertex_input_binding_desc(
    0, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(vertices[0])
  );

  VkVertexInputAttributeDescription vi_attribs[2];
  vi_attribs[0] = wlu_set_vertex_input_attrib_desc(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_2D, pos));
  vi_attribs[1] = wlu_set_vertex_input_attrib_desc(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_2D, color));

  VkPipelineVertexInputStateCreateInfo vertex_input_info = wlu_set_vertex_input_state_info(
    1, &vi_binding, 2, vi_attribs
  );

  /* End of vertex buffer */

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, shi_frag.bytes, shi_frag.byte_size);
  if (!frag_shader_module) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, shi_vert.bytes, shi_vert.byte_size);
  if (!vert_shader_module) {
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }

  wlu_add_watchme_info(1, app, 0, NULL, 1, &frag_shader_module);
  wlu_add_watchme_info(1, app, 0, NULL, 2, &vert_shader_module);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = wlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = wlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info, frag_shader_stage_info
  };

  VkDynamicState dynamic_states[2] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = wlu_set_dynamic_state_info(2, dynamic_states);

  VkPipelineInputAssemblyStateCreateInfo input_assembly = wlu_set_input_assembly_state_info(
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
  );

  VkViewport viewport = wlu_set_view_port(0.0f, 0.0f, (float) extent2D.width, (float) extent2D.height, 0.0f, 1.0f);
  VkRect2D scissor = wlu_set_rect2D(0, 0, extent2D.width, extent2D.height);
  VkPipelineViewportStateCreateInfo view_port_info = wlu_set_view_port_state_info(1, &viewport, 1, &scissor);

  VkPipelineRasterizationStateCreateInfo rasterizer = wlu_set_rasterization_state_info(
    VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
    VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
  );

  VkPipelineMultisampleStateCreateInfo multisampling = wlu_set_multisample_state_info(
    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE
  );

  VkPipelineColorBlendAttachmentState color_blend_attachment = wlu_set_color_blend_attachment_state(
    VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  );

  float blend_const[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  VkPipelineColorBlendStateCreateInfo color_blending = wlu_set_color_blend_attachment_state_info(
    VK_TRUE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment, blend_const
  );

  err = wlu_create_pipeline_cache(app, 0, NULL);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_create_pipeline_cache failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_graphics_pipeline(app, 2, shader_stages,
    &vertex_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, VK_NULL_HANDLE, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create graphics pipeline");
    ck_abort_msg(NULL);
  }

  wlu_log_me(WLU_SUCCESS, "graphics pipeline creation successfull");
  wlu_freeup_shader(app, &frag_shader_module);
  wlu_freeup_shader(app, &vert_shader_module);

  /* Ending setup for graphics pipeline */

  float float32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  int32_t int32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  uint32_t uint32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  VkClearValue clear_value = wlu_set_clear_value(float32, int32, uint32, 0.0f, 0);

  wlu_retrieve_device_queue(app);

  wlu_exec_begin_render_pass(app, 0, 0, extent2D.width, extent2D.height,
                             1, &clear_value, VK_SUBPASS_CONTENTS_INLINE);
  wlu_bind_gp(app, cur_buff, VK_PIPELINE_BIND_POINT_GRAPHICS);

  for (uint32_t i = 0; i < app->bdc; i++) {
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].name: %s", i, app->buffs_data[i].name);
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].buff: %p - %p", i, &app->buffs_data[i].buff, app->buffs_data[i].buff);
  }

  const VkDeviceSize offsets[] = {0};
  wlu_bind_vertex_buff_to_cmd_buffs(app, cur_buff, 0, 1, &app->buffs_data[0].buff, offsets);
  wlu_cmd_set_viewport(app, viewport, cur_buff, 0, 1);
  wlu_cmd_draw(app, cur_buff, 3, 1, 0, 0);

  wlu_exec_stop_render_pass(app);
  err = wlu_exec_stop_cmd_buffs(app);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_exec_queue_cmd_buff failed");
    ck_abort_msg(NULL);
  }

  VkPipelineStageFlags wait_stages[2] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore wait_semaphores[1] = {app->sems[cur_buff].image};
  VkSemaphore signal_semaphores[1] = {app->sems[cur_buff].render};
  VkCommandBuffer cmd_buffs[1] = {app->cmd_buffs[cur_buff]};
  err = wlu_queue_graphics_queue(app, 1, cmd_buffs, 1, wait_semaphores, wait_stages, 1, signal_semaphores);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_exec_queue_cmd_buff failed");
    ck_abort_msg(NULL);
  }

  err = wlu_queue_present_queue(app, 0, NULL, 1, &app->swap_chain, &cur_buff, NULL);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_exec_queue_cmd_buff failed");
    ck_abort_msg(NULL);
  }

  wait_seconds(1);
  freeme(app, wc);
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
