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
#define LUCUR_MATH_API
#define LUCUR_SPIRV_API
#include <lucom.h>

#include "wayland/client.h"
#include "test-extras.h"
#include "test-shade.h"

#define WIDTH 800
#define HEIGHT 600

static dlu_otma_mems ma = {
  .vkcomp_cnt = 1, .gp_cnt = 1, .si_cnt = 5,
  .scd_cnt = 1, .gpd_cnt = 1, .cmdd_cnt = 1,
  .bd_cnt = 2, .ld_cnt = 1, .pd_cnt = 1
};

static bool init_buffs(vkcomp *app) {
  bool err;

  err = dlu_otba(DLU_PD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_LD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_BUFF_DATA, app, INDEX_IGNORE, 2);
  if (!err) return err;

  err = dlu_otba(DLU_SC_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_GP_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_CMD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  return err;
}

START_TEST(test_vulkan_client_create) {
  VkResult err;

  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  wclient *wc = dlu_init_wc();
  check_err(!wc, NULL, NULL, NULL)

  vkcomp *app = dlu_init_vk();
  check_err(!app, NULL, wc, NULL)

  err = init_buffs(app);
  check_err(!err, app, wc, NULL)

  err = dlu_create_instance(app, "Hello Triangle", "No Engine", ARR_LEN(enabled_validation_layers), enabled_validation_layers, ARR_LEN(instance_extensions), instance_extensions);
  check_err(err, app, wc, NULL)

  VkDebugUtilsMessageSeverityFlagsEXT messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  VkDebugUtilsMessageTypeFlagsEXT messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  err = dlu_set_debug_message(app, 0, messageSeverity, messageType);
  check_err(err, app, wc, NULL)

  check_err(!dlu_create_client(wc), app, wc, NULL)

  /* initialize vulkan app surface */
  err = dlu_create_vkwayland_surfaceKHR(app, wc->display, wc->surface);
  check_err(err, app, wc, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  uint32_t cur_ld = 0, cur_pd = 0;
  err = dlu_create_physical_device(app, cur_pd, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, wc, NULL)

  err = dlu_create_queue_families(app, cur_pd, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, wc, NULL)

  float queue_priorities[1] = {1.0};
  VkDeviceQueueCreateInfo dqueue_create_info[1];
  dqueue_create_info[0] = dlu_set_device_queue_info(0, app->pd_data[cur_pd].gfam_idx, 1, queue_priorities);

  err = dlu_create_logical_device(app, cur_pd, cur_ld, 0, ARR_LEN(dqueue_create_info), dqueue_create_info, &device_feats, ARR_LEN(device_extensions), device_extensions);
  check_err(err, app, wc, NULL)

  err = dlu_create_device_queue(app, cur_ld, 0, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, wc, NULL)

  VkSurfaceCapabilitiesKHR capabilities = dlu_get_physical_device_surface_capabilities(app, cur_pd);
  check_err(capabilities.minImageCount == UINT32_MAX, app, wc, NULL)

  /**
  * VK_FORMAT_B8G8R8A8_UNORM will store the R, G, B and alpha channels
  * in that order with an 8 bit unsigned integer and a total of 32 bits per pixel.
  * SRGB if used for colorSpace if available, because it
  * results in more accurate perceived colors
  */
  VkSurfaceFormatKHR surface_fmt = dlu_choose_swap_surface_format(app, cur_pd, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  check_err(surface_fmt.format == VK_FORMAT_UNDEFINED, app, wc, NULL)

  VkPresentModeKHR pres_mode = dlu_choose_swap_present_mode(app, cur_pd);
  check_err(pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR, app, wc, NULL)

  VkExtent2D extent2D = dlu_choose_swap_extent(capabilities, WIDTH, HEIGHT);
  check_err(extent2D.width == UINT32_MAX, app, wc, NULL)

  uint32_t cur_buff = 0, cur_scd = 0, cur_pool = 0, cur_gpd = 0, cur_bd = 0, cur_cmdd = 0;
  err = dlu_otba(DLU_SC_DATA_MEMS, app, cur_scd, capabilities.minImageCount);
  check_err(!err, app, wc, NULL)

  /* image is owned by one queue family at a time, Best for performance */
  VkSwapchainCreateInfoKHR swapchain_info = dlu_set_swap_chain_info(NULL, 0, app->surface, app->sc_data[cur_scd].sic, surface_fmt.format, surface_fmt.colorSpace,
    extent2D, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, capabilities.supportedTransforms, capabilities.supportedCompositeAlpha,
    pres_mode, VK_FALSE, VK_NULL_HANDLE
  );

  /* describe what the image's purpose is and which part of the image should be accessed */
  VkComponentMapping comp_map = dlu_set_component_mapping(VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A);
  VkImageSubresourceRange img_sub_rr = dlu_set_image_sub_resource_range(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
  VkImageViewCreateInfo img_view_info = dlu_set_image_view_info(0, VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_2D, surface_fmt.format, comp_map, img_sub_rr);

  err = dlu_create_swap_chain(app, cur_ld, cur_scd, &swapchain_info, &img_view_info);
  check_err(err, app, wc, NULL)

  err = dlu_create_cmd_pool(app, cur_ld, cur_scd, cur_cmdd, app->pd_data[cur_pd].gfam_idx, 0);
  check_err(err, app, wc, NULL)

  err = dlu_create_cmd_buffs(app, cur_pool, cur_scd, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  check_err(err, app, wc, NULL)

  /* This is where creation of the graphics pipeline begins */
  err = dlu_create_syncs(app, cur_scd);
  check_err(err, app, wc, NULL)

  err = dlu_create_pipeline_layout(app, cur_ld, cur_gpd, 0, NULL, 0, NULL, 0);
  check_err(err, app, wc, NULL)

  /* Starting point for render pass creation */
  VkAttachmentDescription color_attachment = dlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  VkAttachmentReference color_attachment_ref = dlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkSubpassDescription subpass = dlu_set_subpass_desc(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &color_attachment_ref, NULL, NULL, 0, NULL);

  VkSubpassDependency subdep = dlu_set_subpass_dep(VK_SUBPASS_EXTERNAL, 0,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0
  );

  err = dlu_create_render_pass(app, cur_gpd, 1, &color_attachment, 1, &subpass, 1, &subdep, 0);
  check_err(err, app, wc, NULL)

  dlu_log_me(DLU_SUCCESS, "Successfully created render pass");
  /* ending point for render pass creation */

  VkImageView vkimg_attach[1];
  err = dlu_create_framebuffers(app, cur_scd, cur_gpd, 1, vkimg_attach, extent2D.width, extent2D.height, 1);
  check_err(err, app, wc, NULL)

  err = dlu_create_pipeline_cache(app, cur_ld, 0, NULL);
  check_err(err, app, wc, NULL)

  /* 0 is the binding. The # of bytes there is between successive structs */
  VkVertexInputBindingDescription vi_binding = dlu_set_vertex_input_binding_desc(0, sizeof(vertex_2D), VK_VERTEX_INPUT_RATE_VERTEX);

  VkVertexInputAttributeDescription vi_attribs[2];
  vi_attribs[0] = dlu_set_vertex_input_attrib_desc(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_2D, pos));
  vi_attribs[1] = dlu_set_vertex_input_attrib_desc(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_2D, color));

  VkPipelineVertexInputStateCreateInfo vertex_input_info = dlu_set_vertex_input_state_info(
    1, &vi_binding, ARR_LEN(vi_attribs), vi_attribs
  );

  dlu_log_me(DLU_INFO, "Start of shader creation");
  dlu_log_me(DLU_WARNING, "Compiling the fragment shader code to spirv bytes");
  dlu_shader_info shi_frag = dlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT, shader_frag_src, "frag.spv", "main");
  check_err(!shi_frag.bytes, app, wc, NULL)

  dlu_log_me(DLU_WARNING, "Compiling the vertex shader code into spirv bytes");
  dlu_shader_info shi_vert = dlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT, shader_vert_src, "vert.spv", "main");
  check_err(!shi_vert.bytes, app, wc, NULL)

  VkShaderModule frag_shader_module = dlu_create_shader_module(app, cur_ld, shi_frag.bytes, shi_frag.byte_size);
  check_err(!frag_shader_module, app, wc, NULL)

  VkShaderModule vert_shader_module = dlu_create_shader_module(app, cur_ld, shi_vert.bytes, shi_vert.byte_size);
  check_err(!vert_shader_module, app, wc, frag_shader_module)

  dlu_freeup_spriv_bytes(DLU_LIB_SHADERC_SPRIV, shi_vert.result);
  dlu_freeup_spriv_bytes(DLU_LIB_SHADERC_SPRIV, shi_frag.result);
  dlu_log_me(DLU_INFO, "End of shader creation");

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = dlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL, 0
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = dlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL, 0
  );

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info, frag_shader_stage_info
  };

  VkDynamicState dynamic_states[2] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = dlu_set_dynamic_state_info(2, dynamic_states);

  VkPipelineInputAssemblyStateCreateInfo input_assembly = dlu_set_input_assembly_state_info(0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

  VkViewport viewport = dlu_set_view_port(0.0f, 0.0f, (float) extent2D.width, (float) extent2D.height, 0.0f, 1.0f);
  VkRect2D scissor = dlu_set_rect2D(0, 0, extent2D.width, extent2D.height);
  VkPipelineViewportStateCreateInfo view_port_info = dlu_set_view_port_state_info(1, &viewport, 1, &scissor);

  VkPipelineRasterizationStateCreateInfo rasterizer = dlu_set_rasterization_state_info(
    VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
    VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
  );

  VkPipelineMultisampleStateCreateInfo multisampling = dlu_set_multisample_state_info(
    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE
  );

  VkPipelineColorBlendAttachmentState color_blend_attachment = dlu_set_color_blend_attachment_state(
    VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  );

  float blend_const[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  VkPipelineColorBlendStateCreateInfo color_blending = dlu_set_color_blend_attachment_state_info(
    VK_TRUE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment, blend_const
  );

  err = dlu_otba(DLU_GP_DATA_MEMS, app, cur_gpd, 1);
  check_err(!err, app, wc, NULL)

  err = dlu_create_graphics_pipelines(app, cur_gpd, ARR_LEN(shader_stages), shader_stages,
    &vertex_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, VK_NULL_HANDLE, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  check_err(err, NULL, NULL, vert_shader_module)
  check_err(err, app, wc, frag_shader_module)

  dlu_log_me(DLU_SUCCESS, "graphics pipeline creation successfull");
  dlu_vk_destroy(DLU_DESTROY_VK_SHADER, app, cur_ld, frag_shader_module); frag_shader_module = VK_NULL_HANDLE;
  dlu_vk_destroy(DLU_DESTROY_VK_SHADER, app, cur_ld, vert_shader_module); vert_shader_module = VK_NULL_HANDLE;
  /* Ending setup for graphics pipeline */

  /* Start of staging buffer for vertex */
  vertex_2D tri_verts[3] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };
  /* Let the compiler get the size of your array for you. Don't hard code */
  VkDeviceSize vsize = sizeof(tri_verts);
  const uint32_t vertex_count = ARR_LEN(tri_verts);

  const VkDeviceSize offsets[1] = {0};

  for (uint32_t i = 0; i < vertex_count; i++) {
    dlu_print_vector(DLU_VEC2, &tri_verts[i].pos);
    dlu_print_vector(DLU_VEC3, &tri_verts[i].color);
  }

  /**
  * Only creating stagging buffer as a proof of concept
  * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
  * The VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT communicates that the memory
  * should be mapped so that the CPU (host) can access it.
  * The VK_MEMORY_PROPERTY_HOST_COHERENT_BIT requests that the
  * writes to the memory by the host are visible to the device
  * (and vice-versa) without the need to flush memory caches.
  */
  err = dlu_create_vk_buffer(app, cur_ld, cur_bd, vsize, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
    0, NULL, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
  );
  check_err(err, app, wc, NULL)

  err = dlu_vk_map_mem(DLU_VK_BUFFER, app, cur_bd, vsize, tri_verts, 0, 0);
  check_err(err, app, wc, NULL)
  cur_bd++;
  /* End of staging buffer for vertex */

  /* Start of vertex buffer */
  err = dlu_create_vk_buffer(app, cur_ld, cur_bd, vsize, 0,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  check_err(err, app, wc, NULL)
  cur_bd++;

  VkCommandBuffer cmd_buff = dlu_exec_begin_single_time_cmd_buff(app, cur_pool);
  check_err(!cmd_buff, app, wc, NULL)

  /* Used to show functionality */
  dlu_exec_copy_buffer(app, cur_bd-2, cur_bd-1, 0, 0, vsize, cmd_buff);
  /* End of vertex buffer */

  err = dlu_exec_end_single_time_cmd_buff(app, cur_pool, &cmd_buff);
  check_err(err, app, wc, NULL)

  /* Destroy staging buffer as it is no longer needed */
  dlu_vk_destroy(DLU_DESTROY_VK_BUFFER, app, cur_ld, app->buff_data[cur_bd-2].buff); app->buff_data[cur_bd-2].buff = VK_NULL_HANDLE;
  dlu_vk_destroy(DLU_DESTROY_VK_MEMORY, app, cur_ld, app->buff_data[cur_bd-2].mem); app->buff_data[cur_bd-2].mem = VK_NULL_HANDLE;

  float float32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  int32_t int32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  uint32_t uint32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  VkClearValue clear_value = dlu_set_clear_value(float32, int32, uint32, 0.0f, 0);

  /* Set command buffers into recording state */
  err = dlu_exec_begin_cmd_buffs(app, cur_pool, cur_scd, 0, NULL);
  check_err(err, app, wc, NULL)

  /* Drawing will start when you begin a render pass */
  dlu_exec_begin_render_pass(app, cur_pool, cur_scd, cur_gpd, 0, 0, extent2D.width, extent2D.height, 1, &clear_value, VK_SUBPASS_CONTENTS_INLINE);
 
  dlu_exec_cmd_set_viewport(app, &viewport, cur_pool, cur_buff, 0, 1);
  dlu_bind_pipeline(app, cur_pool, cur_buff, cur_gpd, 0, VK_PIPELINE_BIND_POINT_GRAPHICS);
  dlu_bind_vertex_buff_to_cmd_buff(app, cur_pool, cur_buff, cur_bd-1, 0, offsets);
  dlu_exec_cmd_draw(app, cur_pool, cur_buff, vertex_count, 1, 0, 0);

  dlu_exec_stop_render_pass(app, cur_pool, cur_scd);
  err = dlu_exec_stop_cmd_buffs(app, cur_pool, cur_scd);
  check_err(err, app, wc, NULL)

  VkPipelineStageFlags pipe_stage_flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore acquire_sems[1] = {app->sc_data[cur_scd].syncs[0].sem.image};
  VkSemaphore render_sems[1] = {app->sc_data[cur_scd].syncs[0].sem.render};
  VkCommandBuffer cmd_buffs[1] = {app->cmd_data[cur_pool].cmd_buffs[cur_buff]};

  uint32_t cur_img, cur_frame = 0;
  err = dlu_acquire_sc_image_index(app, cur_scd, cur_frame, &cur_img);
  check_err(err, app, wc, NULL)
 
  /* set fence to unsignal state */
  err = dlu_vk_sync(DLU_VK_RESET_RENDER_FENCE, app, cur_scd, 0);
  check_err(err, app, wc, NULL)

  err = dlu_queue_graphics_queue(app, cur_scd, 0, 1, cmd_buffs, 1, acquire_sems, pipe_stage_flags, 1, render_sems);
  check_err(err, app, wc, NULL)

  err = dlu_queue_present_queue(app, cur_ld, 1, render_sems, 1, &app->sc_data[cur_scd].swap_chain, &cur_buff, NULL);
  check_err(err, app, wc, NULL)

  sleep(1);
  FREEME(app, wc)
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestTriangle");

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
