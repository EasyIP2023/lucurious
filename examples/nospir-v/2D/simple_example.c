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

#include <wlu/vkall.h>
#include <wlu/client.h>
#include <wlu/log.h>
#include <wlu/errors.h>
#include <wlu/shade.h>
#include <wlu/gp.h>

#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "simple_example.h"

#define WIDTH 1920
#define HEIGHT 1080

void freeme(vkcomp *app, wclient *wc, wlu_shader_info *shinfo, wlu_shader_info *shinfo_two) {
  wlu_freeup_shi(shinfo);
  wlu_freeup_shi(shinfo_two);
  wlu_freeup_vk(app);
  wlu_freeup_wc(wc);
}

int main(void) {
  VkResult err;

  wclient *wc = wlu_init_wc();
  if (!wc) {
    wlu_log_me(WLU_DANGER, "[x] wlu_init_wc failed!!");
    return EXIT_FAILURE;
  }

  vkcomp *app = wlu_init_vk();
  if (!app) {
    freeme(NULL, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    return EXIT_FAILURE;
  }

  /* Signal handler for this process */
  err = wlu_watch_me(SIGSEGV, getpid());
  if (err) {
    freeme(app, wc, NULL, NULL);
    return EXIT_FAILURE;
  }

  wlu_add_watchme_info(1, app, 1, wc, 0, NULL, 0, NULL);

  err = wlu_set_global_layers(app);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    return EXIT_FAILURE;
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine", 3, enabled_validation_layers, 4, instance_extensions);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    return EXIT_FAILURE;
  }

  err = wlu_set_debug_message(app, 1);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to setup debug message");
    return EXIT_FAILURE;
  }

  err = wlu_enumerate_devices(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    return EXIT_FAILURE;
  }

  if (wlu_connect_client(wc)) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to connect client");
    return EXIT_FAILURE;
  }

  /* initialize vulkan app surface */
  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to connect to vulkan surfaceKHR");
    return EXIT_FAILURE;
  }

  err = wlu_set_queue_family(app, VK_QUEUE_GRAPHICS_BIT);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to set device queue family");
    return EXIT_FAILURE;
  }

  err = wlu_create_logical_device(app, 3, enabled_validation_layers, 1, device_extensions);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to initialize logical device to physical device");
    return EXIT_FAILURE;
  }

  VkSurfaceCapabilitiesKHR capabilities = wlu_q_device_capabilities(app);
  if (capabilities.minImageCount == UINT32_MAX) {
    freeme(app, wc, NULL, NULL);
    return EXIT_FAILURE;
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
    return EXIT_FAILURE;
  }

  VkPresentModeKHR pres_mode = wlu_choose_swap_present_mode(app);
  if (pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    freeme(app, wc, NULL, NULL);
    return EXIT_FAILURE;
  }

  VkExtent3D extent3D = { UINT32_MAX, UINT32_MAX, UINT32_MAX };
  VkExtent2D extent2D = wlu_choose_2D_swap_extent(capabilities, WIDTH, HEIGHT);
  if (extent2D.width == UINT32_MAX) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent2D.width equals %d", extent2D.width);
    return EXIT_FAILURE;
  }

  wlu_retrieve_device_queue(app);

  err = wlu_create_swap_chain(app, capabilities, surface_fmt, pres_mode, extent2D, extent3D);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create swap chain");
    return EXIT_FAILURE;
  }

  err = wlu_create_img_views(app, surface_fmt.format, VK_IMAGE_VIEW_TYPE_2D);
  if (err) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create image views");
    return EXIT_FAILURE;
  }

  /* This is where creation of the graphics pipeline begins */

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
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] failed to create render pass");
    return EXIT_FAILURE;
  }
  wlu_log_me(WLU_SUCCESS, "Successfully created render pass");
  /* ending point for render pass creation */

  wlu_log_me(WLU_WARNING, "Compiling the frag code to spirv shader");

  wlu_shader_info shi_frag = wlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT,
                             shader_frag_src, "frag.spv", "main");
  if (!shi_frag.bytes) {
    freeme(app, wc, NULL, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    return EXIT_FAILURE;
  }

  wlu_log_me(WLU_WARNING, "Compiling the vert code to spirv shader");
  wlu_shader_info shi_vert = wlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT,
                             shader_vert_src, "vert.spv", "main");
  if (!shi_vert.bytes) {
    freeme(app, wc, &shi_frag, NULL);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    return EXIT_FAILURE;
  }

  wlu_add_watchme_info(0, NULL, 0, NULL, 0, NULL, 1, &shi_frag);
  wlu_add_watchme_info(0, NULL, 0, NULL, 0, NULL, 2, &shi_vert);

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, shi_vert.bytes, shi_vert.byte_size);
  if (!vert_shader_module) {
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    return EXIT_FAILURE;
  }

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, shi_frag.bytes, shi_frag.byte_size);
  if (!frag_shader_module) {
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    return EXIT_FAILURE;
  }

  wlu_add_watchme_info(1, app, 0, NULL, 1, &vert_shader_module, 0, NULL);
  wlu_add_watchme_info(1, app, 0, NULL, 2, &frag_shader_module, 0, NULL);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = wlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = wlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info, frag_shader_stage_info
  };

  VkPipelineVertexInputStateCreateInfo vertext_input_info = wlu_set_vertex_input_state_info(
    0, NULL, 0, NULL
  );

  VkPipelineInputAssemblyStateCreateInfo input_assembly = wlu_set_input_assembly_state_info(
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
  );

  VkViewport viewport = wlu_set_view_port(0.0f, 0.0f, (float) extent2D.width, (float) extent2D.height, 0.0f, 1.0f);

  VkRect2D scissor = wlu_set_rect2D(0, 0, extent2D);

  VkPipelineViewportStateCreateInfo view_port_info = wlu_set_view_port_state_info(&viewport, 1, &scissor, 1);

  VkPipelineRasterizationStateCreateInfo rasterizer = wlu_set_rasterization_state_info(
    VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
    VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
  );

  VkPipelineMultisampleStateCreateInfo multisampling = wlu_set_multisample_state_info(
    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE
  );

  VkPipelineColorBlendAttachmentState color_blend_attachment = wlu_set_color_blend_attachment_state(
    VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  );

  float blend_const[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  VkPipelineColorBlendStateCreateInfo color_blending = wlu_set_color_blend_attachment_state_info(
    VK_FALSE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment, blend_const
  );

  VkDynamicState dynamic_states[2] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = wlu_set_dynamic_state_info(2, dynamic_states);

  err = wlu_create_pipeline_layout(app, 0, NULL);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create pipeline layout");
    return EXIT_FAILURE;
  }

  err = wlu_create_graphics_pipeline(app, 2, shader_stages,
    &vertext_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, VK_NULL_HANDLE, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create graphics pipeline");
    return EXIT_FAILURE;
  }

  wlu_log_me(WLU_SUCCESS, "graphics pipeline creation successfull");

  /* Ending setup for graphics pipeline */

  err = wlu_create_framebuffers(app, 1, extent2D, 1);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create framebuffers, ERROR CODE: %d", err);
    return EXIT_FAILURE;
  }

  err = wlu_create_cmd_pool(app, 0);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create command pool, ERROR CODE: %d", err);
    return EXIT_FAILURE;
  }

  err = wlu_create_cmd_buffs(app, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create command buffers, ERROR CODE: %d", err);
    return EXIT_FAILURE;
  }

  err = wlu_create_semaphores(app);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to create semaphores");
    return EXIT_FAILURE;
  }

  err = wlu_exec_begin_cmd_buff(app, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, NULL);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    wlu_log_me(WLU_DANGER, "[x] failed to start command buffer recording");
    return EXIT_FAILURE;
  }

  VkClearValue clear_color;
  clear_color.color.float32[0] = 0.0f;
  clear_color.color.float32[1] = 0.0f;
  clear_color.color.float32[2] = 0.0f;
  clear_color.color.float32[3] = 1.0f;
  clear_color.color.int32[0] = 0.0f;
  clear_color.color.int32[1] = 0.0f;
  clear_color.color.int32[2] = 0.0f;
  clear_color.color.int32[3] = 1.0f;
  clear_color.color.uint32[0] = 0.0f;
  clear_color.color.uint32[1] = 0.0f;
  clear_color.color.uint32[2] = 0.0f;
  clear_color.color.uint32[3] = 1.0f;
  clear_color.depthStencil.depth = 0.0f;
  clear_color.depthStencil.stencil = 0;

  wlu_exec_begin_render_pass(app, 0, 0, extent2D, 1, &clear_color, VK_SUBPASS_CONTENTS_INLINE);

  wlu_bind_gp(app, VK_PIPELINE_BIND_POINT_GRAPHICS);
  wlu_draw(app, 3, 1, 0, 0);

  err = wlu_run_client(wc);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    return EXIT_FAILURE;
  }

  err = wlu_exec_stop_cmd_buff(app);
  if (err) {
    wlu_freeup_shader(app, &frag_shader_module);
    wlu_freeup_shader(app, &vert_shader_module);
    freeme(app, wc, &shi_frag, &shi_vert);
    return EXIT_FAILURE;
  }

  wlu_exec_stop_render_pass(app);

  wlu_freeup_shader(app, &frag_shader_module);
  wlu_freeup_shader(app, &vert_shader_module);
  freeme(app, wc, &shi_frag, &shi_vert);

  return EXIT_SUCCESS;
}
