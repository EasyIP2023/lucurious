/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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
#include <time.h>

#define LUCUR_VKCOMP_API
#define LUCUR_VKCOMP_MATRIX_API
#define LUCUR_WAYLAND_API
#define LUCUR_WAYLAND_CLIENT_API
#define LUCUR_SHADE_API
#include <lucom.h>

#include "test-extras.h"
#include "test-shade.h"

#define WIDTH 800
#define HEIGHT 600

#define drand48() ((float)(rand() / (RAND_MAX + 1.0)))

static wlu_otma_mems ma = {
  .vkcomp_cnt = 10, .wclient_cnt = 10, .desc_cnt = 10,
  .gp_cnt = 10, .si_cnt = 15, .scd_cnt = 10, .gpd_cnt = 10,
  .cmdd_cnt = 10, .bd_cnt = 10, .dd_cnt = 10
};

/* Be sure to make struct binary compatible with shader variable */
struct uniform_block_data {
  mat4 model;
  mat4 view;
  mat4 proj;
};

static VkResult init_buffs(vkcomp *app) {
  VkResult err;

  err = wlu_otba(app, ALLOC_INDEX_NON, 9, WLU_BUFFS_DATA);
  if (err) return err;

  err = wlu_otba(app, ALLOC_INDEX_NON, 1, WLU_SC_DATA);
  if (err) return err;

  err = wlu_otba(app, ALLOC_INDEX_NON, 1, WLU_GP_DATA);
  if (err) return err;

  err = wlu_otba(app, ALLOC_INDEX_NON, 1, WLU_CMD_DATA);
  if (err) return err;

  err = wlu_otba(app, ALLOC_INDEX_NON, 1, WLU_DESC_DATA);
  if (err) return err;

  return err;
}

START_TEST(test_vulkan_client_create) {
  VkResult err;

  if (!wlu_otma(WLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  wclient *wc = wlu_init_wc();
  check_err(!wc, NULL, NULL, NULL)

  vkcomp *app = wlu_init_vk();
  check_err(!app, NULL, wc, NULL)

  err = init_buffs(app);
  check_err(err, app, wc, NULL)

  err = wlu_create_instance(app, "Hello Triangle", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, wc, NULL)

  err = wlu_set_debug_message(app);
  check_err(err, app, wc, NULL)

  check_err(!wlu_create_client(wc), app, wc, NULL)

  /* initialize vulkan app surface */
  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  check_err(err, app, wc, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = wlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, wc, NULL)

  err = wlu_set_queue_family(app, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, wc, NULL)

  err = wlu_create_logical_device(app, &device_feats, 1, 1, enabled_validation_layers, 1, device_extensions);
  check_err(err, app, wc, NULL)

  VkSurfaceCapabilitiesKHR capabilities = wlu_q_device_capabilities(app);
  check_err(capabilities.minImageCount == UINT32_MAX, app, wc, NULL)

  /**
  * VK_FORMAT_B8G8R8A8_UNORM will store the R, G, B and alpha channels
  * in that order with an 8 bit unsigned integer and a total of 32 bits per pixel.
  * SRGB if used for colorSpace if available, because it
  * results in more accurate perceived colors
  */
  VkSurfaceFormatKHR surface_fmt = wlu_choose_swap_surface_format(app, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  check_err(surface_fmt.format == VK_FORMAT_UNDEFINED, app, wc, NULL)

  VkPresentModeKHR pres_mode = wlu_choose_swap_present_mode(app);
  check_err(pres_mode == VK_PRESENT_MODE_MAX_ENUM_KHR, app, wc, NULL)

  VkExtent2D extent2D = wlu_choose_2D_swap_extent(capabilities, WIDTH, HEIGHT);
  check_err(extent2D.width == UINT32_MAX, app, wc, NULL)

  uint32_t cur_buff = 0, cur_scd = 0, cur_pool = 0, cur_gpd = 0, cur_bd = 0, cur_cmdd = 0, cur_dd = 0;
  err = wlu_otba(app, cur_scd, capabilities.minImageCount, WLU_SC_DATA_MEMS);
  check_err(err, app, wc, NULL)

  /* Does not check if image count exceeds the max */
  err = wlu_create_swap_chain(app, cur_scd, capabilities, surface_fmt, pres_mode, extent2D.width, extent2D.height);
  check_err(err, app, wc, NULL)

  err = wlu_create_cmd_pool(app, cur_scd, cur_cmdd, app->indices.graphics_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  check_err(err, app, wc, NULL)

  err = wlu_create_cmd_buffs(app, cur_pool, cur_scd, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  check_err(err, app, wc, NULL)

  err = wlu_create_img_views(app, cur_scd, surface_fmt.format, VK_IMAGE_VIEW_TYPE_2D);
  check_err(err, app, wc, NULL)

  /* This is where creation of the graphics pipeline begins */
  err = wlu_create_semaphores(app, cur_scd);
  check_err(err, app, wc, NULL)

  /* Starting point for render pass creation */
  VkAttachmentDescription attachment = wlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  VkAttachmentReference color_ref = wlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  VkSubpassDescription subpass = wlu_set_subpass_desc(
    0, NULL, 1, &color_ref, NULL, NULL, 0, NULL
  );

  VkSubpassDependency subdep = wlu_set_subpass_dep(VK_SUBPASS_EXTERNAL, 0,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0
  );

  err = wlu_create_render_pass(app, cur_gpd, 1, &attachment, 1, &subpass, 1, &subdep);
  check_err(err, app, wc, NULL)

  wlu_log_me(WLU_SUCCESS, "Successfully created render pass");
  /* ending point for render pass creation */

  wlu_log_me(WLU_WARNING, "Compiling the frag code to spirv shader");

  wlu_shader_info shi_frag = wlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT,
                             spin_square_frag_text, "frag.spv", "main");
  check_err(!shi_frag.bytes, app, wc, NULL)

  wlu_log_me(WLU_WARNING, "Compiling the vert code to spirv shader");
  wlu_shader_info shi_vert = wlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT,
                             spin_square_vert_text, "vert.spv", "main");
  check_err(!shi_vert.bytes, app, wc, NULL)

  VkImageView vkimg_attach[1];
  err = wlu_create_framebuffers(app, cur_scd, cur_gpd, 1, vkimg_attach, extent2D.width, extent2D.height, 1);
  check_err(err, app, wc, NULL)

  err = wlu_create_pipeline_cache(app, 0, NULL);
  check_err(err, app, wc, NULL)

  /* 0 is the binding. The # of is bytes there is between successive structs */
  VkVertexInputBindingDescription vi_binding = wlu_set_vertex_input_binding_desc(0, sizeof(vertex_2D), VK_VERTEX_INPUT_RATE_VERTEX);

  VkVertexInputAttributeDescription vi_attribs[2];
  vi_attribs[0] = wlu_set_vertex_input_attrib_desc(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_2D, pos));
  vi_attribs[1] = wlu_set_vertex_input_attrib_desc(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_2D, color));

  VkPipelineVertexInputStateCreateInfo vertex_input_info = wlu_set_vertex_input_state_info(1, &vi_binding, 2, vi_attribs);

  err = wlu_otba(app, cur_dd, app->sc_data[cur_scd].sic, WLU_DESC_DATA_MEMS);
  check_err(err, app, wc, NULL)

  /* MVP transformation is in a single uniform buffer variable (not an array), So descriptor count is 1 */
  VkDescriptorSetLayoutBinding desc_set = wlu_set_desc_set_layout_binding(
    0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkDescriptorSetLayoutCreateInfo desc_set_info = wlu_set_desc_set_layout_info(0, 1, &desc_set);

  /* Using same layout for all obects for now */
  err = wlu_create_desc_set_layouts(app, cur_dd, &desc_set_info);
  check_err(err, app, wc, NULL)

  err = wlu_create_pipeline_layout(app, cur_gpd, cur_dd, 0, NULL);
  check_err(err, app, wc, NULL)

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, shi_frag.bytes, shi_frag.byte_size);
  check_err(!frag_shader_module, app, wc, NULL)

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, shi_vert.bytes, shi_vert.byte_size);
  check_err(!vert_shader_module, app, wc, frag_shader_module)

  wlu_freeup_spriv_bytes(WLU_LIB_SHADERC_SPRIV, shi_vert.result);
  wlu_freeup_spriv_bytes(WLU_LIB_SHADERC_SPRIV, shi_frag.result);

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
    VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
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

  err = wlu_otba(app, cur_dd, 1, WLU_GP_DATA_MEMS);
  check_err(err, app, wc, NULL)

  err = wlu_create_graphics_pipelines(app, cur_gpd, 2, shader_stages,
    &vertex_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, VK_NULL_HANDLE, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  check_err(err, NULL, NULL, vert_shader_module)
  check_err(err, app, wc, frag_shader_module)

  wlu_log_me(WLU_SUCCESS, "graphics pipeline creation successfull");
  wlu_freeup_shader(app, frag_shader_module); frag_shader_module = VK_NULL_HANDLE;
  wlu_freeup_shader(app, vert_shader_module); vert_shader_module = VK_NULL_HANDLE;

  /* Ending setup for graphics pipeline */

  /* Start of vertex buffer */
  vertex_2D verts[4] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
  };
  VkDeviceSize vsize = sizeof(verts);
  const uint32_t vertex_count = vsize / sizeof(vertex_2D);
  for (uint32_t i = 0; i < vertex_count; i++) {
    wlu_print_vector(&verts[i].pos, WLU_VEC2);
    wlu_print_vector(&verts[i].color, WLU_VEC3);
  }

  err = wlu_create_vk_buffer(app, cur_bd, vsize, 0,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 's',
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, verts);
  check_err(err, app, wc, NULL)
  cur_bd++;

  /**
  * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
  * The VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT communicates that the memory
  * should be mapped so that the CPU (host) can access it.
  * The VK_MEMORY_PROPERTY_HOST_COHERENT_BIT requests that the
  * writes to the memory by the host are visible to the device
  * (and vice-versa) without the need to flush memory caches.
  */
  err = wlu_create_vk_buffer(app, cur_bd, vsize, 0,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'v', VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, NULL);
  check_err(err, app, wc, NULL)
  cur_bd++;

  err = wlu_copy_buffer(app, cur_pool, app->buffs_data[0].buff, app->buffs_data[1].buff, vsize);
  check_err(err, app, wc, NULL)
  /* End of vertex buffer creation */

  /* Start of index buffer creation */
  VkDeviceSize isize = sizeof(indices);
  const uint32_t index_count = isize / sizeof(uint16_t);
  err = wlu_create_vk_buffer(app, cur_bd, isize, 0,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 's',
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, indices);
  check_err(err, app, wc, NULL)
  cur_bd++;

  err = wlu_create_vk_buffer(app, cur_bd, isize, 0,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'i', VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, NULL);
  check_err(err, app, wc, NULL)
  cur_bd++;

  err = wlu_copy_buffer(app, cur_pool, app->buffs_data[2].buff, app->buffs_data[3].buff, isize);
  check_err(err, app, wc, NULL)
  /* End of index buffer creation */

  /* Now Creating uniform buffers */
  for (uint32_t i = cur_bd; i < (cur_bd+app->sc_data[cur_scd].sic); i++) {
    err = wlu_create_vk_buffer(app, i, sizeof(struct uniform_block_data), 0,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'u',
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    check_err(err, app, wc, NULL)
    wlu_log_me(WLU_SUCCESS, "Just Allocated!!!");
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].name: %c", i, app->buffs_data[i].name);
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].buff: %p - %p", i, &app->buffs_data[i].buff, app->buffs_data[i].buff);
  }
  /* Done creating uniform buffers */

  VkDescriptorPoolSize pool_size = wlu_set_desc_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, app->sc_data[cur_scd].sic);
  err = wlu_create_desc_pool(app, cur_dd, 0, 1, &pool_size);
  check_err(err, app, wc, NULL)

  err = wlu_create_desc_set(app, cur_dd);
  check_err(err, app, wc, NULL)

  /* set uniform buffer VKBufferInfos */
  VkDescriptorBufferInfo buff_info;
  VkWriteDescriptorSet write;
  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    buff_info = wlu_set_desc_buff_info(app->buffs_data[i+cur_bd].buff, 0, VK_WHOLE_SIZE);
    write = wlu_write_desc_set(app->desc_data[cur_dd].desc_set[i], 0, 0, 1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &buff_info, NULL);
    wlu_update_desc_sets(app, 1, &write, 0, NULL);
  }

  float float32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  int32_t int32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  uint32_t uint32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  VkClearValue clear_value = wlu_set_clear_value(float32, int32, uint32, 0.0f, 0);

  /* Set command buffers into recording state */
  err = wlu_exec_begin_cmd_buffs(app, cur_pool, cur_scd, 0, NULL);
  check_err(err, app, wc, NULL)

  wlu_log_me(WLU_SUCCESS, "ALL ALLOCATED BUFFERS");
  for (uint32_t i = 0; i < app->bdc; i++) {
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].name: %c", i, app->buffs_data[i].name);
    wlu_log_me(WLU_INFO, "app->buffs_data[%d].buff: %p - %p", i, &app->buffs_data[i].buff, app->buffs_data[i].buff);
  }

  /* Drawing will start when you begin a render pass */
  wlu_exec_begin_render_pass(app, cur_pool, cur_scd, cur_gpd, 0, 0, extent2D.width,
                             extent2D.height, 2, &clear_value, VK_SUBPASS_CONTENTS_INLINE);
  wlu_cmd_set_viewport(app, &viewport, cur_pool, cur_buff, 0, 1);
  wlu_bind_pipeline(app, cur_pool, cur_buff, VK_PIPELINE_BIND_POINT_GRAPHICS, app->gp_data[cur_gpd].graphics_pipelines[0]);
  wlu_bind_desc_sets(app, cur_pool, cur_buff, cur_dd, cur_gpd, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, 0, NULL);

  const VkDeviceSize offsets[1] = {0};
  wlu_bind_vertex_buffs_to_cmd_buff(app, cur_pool, cur_buff, 0, 1, &app->buffs_data[1].buff, offsets);
  wlu_bind_index_buff_to_cmd_buff(app, cur_pool, cur_buff, app->buffs_data[3].buff, offsets[0], VK_INDEX_TYPE_UINT16);

  wlu_cmd_draw_indexed(app, cur_pool, cur_buff, index_count, 1, 0, offsets[0], 0);

  wlu_exec_stop_render_pass(app, cur_pool, cur_scd);
  err = wlu_exec_stop_cmd_buffs(app, cur_pool, cur_scd);
  check_err(err, app, wc, NULL)

  VkPipelineStageFlags wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkCommandBuffer cmd_buffs[1] = {app->cmd_data[cur_pool].cmd_buffs[cur_buff]};

  struct uniform_block_data ubd;

  float fovy = wlu_set_fovy(45.0f);
  float hw = (float) extent2D.width / (float) extent2D.height;

  if (extent2D.width > extent2D.height) fovy *= hw;
  wlu_set_perspective(ubd.proj, fovy, hw, 0.1f, 10.0f);
  ubd.proj[1][1] *= -1; /* Invert Y-Coordinate */

  // uint32_t image_index = 0;
  VkSemaphore image_sems[app->sc_data[cur_scd].sic], render_sems[app->sc_data[cur_scd].sic];

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    image_sems[i] = app->sc_data[cur_scd].sems[i].image;
    render_sems[i] = app->sc_data[cur_scd].sems[i].render;

    err = wlu_acquire_sc_img_index(app, cur_scd, &i);
    check_err(err, app, wc, NULL)

    srand((unsigned int)time(NULL));
    wlu_set_matrix(ubd.model, model_matrix_default, WLU_MAT4);
    wlu_set_rotate(ubd.model, ubd.model, drand48() * wlu_set_fovy(90.0f), WLU_Z);
    wlu_set_lookat(ubd.view, spin_eye, spin_center, spin_up);

    err = wlu_create_buff_mem_map(app, cur_bd+i, &ubd);
    check_err(err, app, wc, NULL)

    err = wlu_queue_graphics_queue(app, 1, cmd_buffs, 1, &image_sems[i], wait_stages, 1, &render_sems[i]);
    check_err(err, app, wc, NULL)

    err = wlu_queue_present_queue(app, 1, &render_sems[i], 1, &app->sc_data[cur_scd].swap_chain, &i, NULL);
    check_err(err, app, wc, NULL)
  }

  sleep(3);
  FREEME(app, wc)
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestSquare");

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

//  sleep(7);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
