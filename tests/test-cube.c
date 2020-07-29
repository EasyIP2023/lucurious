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

#define NUM_DESCRIPTOR_SETS 1
#define WIDTH 800
#define HEIGHT 600
#define DEPTH 1

static dlu_otma_mems ma = {
  .vkcomp_cnt = 1, .desc_cnt = 1, .gp_cnt = 1, .si_cnt = 5,
  .scd_cnt = 1, .gpd_cnt = 1, .cmdd_cnt = 1, .bd_cnt = 1,
  .dd_cnt = 1, .ld_cnt = 1, .pd_cnt = 1
};

static struct uniform_block_data {
  mat4 proj;
  mat4 view;
  mat4 model;
  mat4 clip;
  mat4 mvp;
} ubd;

static void dlu_print_matrices() {
  dlu_log_me(DLU_INFO, "Perspective Matrix");
  dlu_log_me(DLU_INFO, "Projection from camera to screen");
  dlu_print_matrix(DLU_MAT4, ubd.proj);
  dlu_log_me(DLU_INFO, "View Matrix");
  dlu_log_me(DLU_INFO, "View from world space to camera space");
  dlu_print_matrix(DLU_MAT4, ubd.view);
  dlu_log_me(DLU_INFO, "Model Matrix");
  dlu_log_me(DLU_INFO, "Mapping object's local coordinate space into world space");
  dlu_print_matrix(DLU_MAT4, ubd.model);
  dlu_log_me(DLU_INFO, "Clip Matrix");
  dlu_print_matrix(DLU_MAT4, ubd.clip);
  dlu_log_me(DLU_INFO, "MVP Matrix");
  dlu_print_matrix(DLU_MAT4, ubd.mvp);
}

static bool init_buffs(vkcomp *app) {
  bool err;

  err = dlu_otba(DLU_PD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_LD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_BUFF_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_SC_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_GP_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_CMD_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  err = dlu_otba(DLU_DESC_DATA, app, INDEX_IGNORE, 1);
  if (!err) return err;

  return err;
}

START_TEST(test_vulkan_client_create_3D) {
  VkResult err;

  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  wclient *wc = dlu_init_wc();
  check_err(!wc, NULL, NULL, NULL)

  vkcomp *app = dlu_init_vk();
  check_err(!app, NULL, wc, NULL)

  err = init_buffs(app);
  check_err(!err, app, wc, NULL)

  err = dlu_create_instance(app, "Draw Cube", "No Engine", ARR_LEN(enabled_validation_layers), enabled_validation_layers, ARR_LEN(instance_extensions), instance_extensions);
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
  uint32_t cur_pd = 0, cur_ld = 0;
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

  uint32_t cur_buff = 0, cur_scd = 0, cur_pool = 0, cur_dd = 0, cur_gpd = 0, cur_bd = 0, cur_cmd = 0;
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

  err = dlu_create_cmd_pool(app, cur_ld, cur_scd, cur_cmd, app->pd_data[cur_pd].gfam_idx, 0);
  check_err(err, app, wc, NULL)

  err = dlu_create_cmd_buffs(app, cur_pool, cur_scd, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  check_err(err, app, wc, NULL)

  VkExtent3D extend3D = {extent2D.width, extent2D.height, DEPTH};
  VkImageCreateInfo img_info = dlu_set_image_info(0, VK_IMAGE_TYPE_2D, VK_FORMAT_D16_UNORM, extend3D, 1,
    1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_IMAGE_LAYOUT_UNDEFINED
  );

  img_sub_rr = dlu_set_image_sub_resource_range(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1);
  img_view_info = dlu_set_image_view_info(0, VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D16_UNORM, comp_map, img_sub_rr);  
  err = dlu_create_depth_buff(app, cur_scd, &img_info, &img_view_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  check_err(err, app, wc, NULL)

  err = dlu_create_syncs(app, cur_scd);
  check_err(err, app, wc, NULL)

  float fovy = dlu_set_radian(45.0f);
  float hw = (float) extent2D.width / (float) extent2D.height;
  if (extent2D.width > extent2D.height) fovy *= hw;
  dlu_set_perspective(ubd.proj, fovy, hw, 0.1f, 100.0f);
  dlu_set_lookat(ubd.view, eye, center, up);
  dlu_set_matrix(DLU_MAT4_IDENTITY, ubd.model, NULL);
  dlu_set_matrix(DLU_MAT4, ubd.clip, clip_matrix);
  dlu_set_mvp_matrix(ubd.mvp, &ubd.clip, &ubd.proj, &ubd.view, &ubd.model);
  dlu_print_matrices();

  /* Create uniform buffer & vertex buffer that has the transformation matrices (for the vertex shader) */
  VkDeviceSize vsize = sizeof(vertices);
  const uint32_t vertex_count = ARR_LEN(vertices);
  const VkDeviceSize offsets[] = {0, vsize};

  err = dlu_create_vk_buffer(app, cur_ld, cur_bd, sizeof(ubd.mvp)+vsize, 0, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
  );
  check_err(err, app, wc, NULL)

  /* Map vertices into vertex buffer */
  err = dlu_vk_map_mem(DLU_VK_BUFFER, app, cur_bd, vsize, vertices, offsets[0], 0);
  check_err(err, app, wc, NULL)
  
  /* Map mvp matrix into memory. Matrix is binary compatible with shader variable */
  err = dlu_vk_map_mem(DLU_VK_BUFFER, app, cur_bd, sizeof(ubd.mvp), ubd.mvp, offsets[1], 0);
  check_err(err, app, wc, NULL)

  /**
  * MVP transformation is in a single uniform buffer variable (not an array), So descriptor count is 1
  * Specify to X particular graphics pipeline how you plan on utilizing descriptor sets and
  * at what shader stages these descriptor sets operate on. The binding represents the index of
  * a descriptor within a set.
  */
  VkDescriptorSetLayoutBinding binding = dlu_set_desc_set_layout_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL);
  VkDescriptorSetLayoutCreateInfo desc_set_info[1]; desc_set_info[0] = dlu_set_desc_set_layout_info(0, 1, &binding);

  err = dlu_create_pipeline_layout(app, cur_ld, cur_gpd, ARR_LEN(desc_set_info), desc_set_info, 0, NULL, 0);
  check_err(err, app, wc, NULL)

  /* start of render pass creation */
  dlu_log_me(DLU_INFO, "Start of render pass creation");

  VkAttachmentDescription attachments[2];
  /* Create render pass color attachment for swapchain images */
  attachments[0] = dlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  /* Create render pass stencil/depth attachment for depth buffer */
  attachments[1] = dlu_set_attachment_desc(VK_FORMAT_D16_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, 
    VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, 
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  );

  VkAttachmentReference color_ref = dlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkAttachmentReference depth_ref = dlu_set_attachment_ref(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  VkSubpassDescription subpass = dlu_set_subpass_desc(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &color_ref, NULL, &depth_ref, 0, NULL);

  err = dlu_create_render_pass(app, cur_gpd, 2, attachments, 1, &subpass, 0, NULL, 0);
  check_err(err, app, wc, NULL)

  dlu_log_me(DLU_SUCCESS, "Successfully created the render pass!!!");
  /* End of render pass creation */;

  VkImageView vkimg_attach[2];
  vkimg_attach[1] = app->sc_data[cur_scd].depth.view;
  err = dlu_create_framebuffers(app, cur_scd, cur_gpd, 2, vkimg_attach, extent2D.width, extent2D.height, 1);
  check_err(err, app, wc, NULL)

  err = dlu_create_pipeline_cache(app, cur_ld, 0, NULL);
  check_err(err, app, wc, NULL)

  /* 0 is the binding. The # of bytes there is between successive structs */
  VkVertexInputBindingDescription vi_binding = dlu_set_vertex_input_binding_desc(0, sizeof(vertex_3D), VK_VERTEX_INPUT_RATE_VERTEX);

  VkVertexInputAttributeDescription vi_attribs[2];
  vi_attribs[0] = dlu_set_vertex_input_attrib_desc(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex_3D, pos));
  vi_attribs[1] = dlu_set_vertex_input_attrib_desc(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex_3D, color));

  VkPipelineVertexInputStateCreateInfo vertex_input_info = dlu_set_vertex_input_state_info(
    1, &vi_binding, 2, vi_attribs
  );

  dlu_log_me(DLU_INFO, "Start of shader creation");
  dlu_log_me(DLU_WARNING, "Compiling the fragment shader code to spirv bytes");
  dlu_shader_info shi_frag = dlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, "frag.spv", "main");
  check_err(!shi_frag.bytes, app, wc, NULL)

  dlu_log_me(DLU_WARNING, "Compiling the vertex shader code into spirv bytes");
  dlu_shader_info shi_vert = dlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, "vert.spv", "main");
  check_err(!shi_vert.bytes, app, wc, NULL)
  dlu_log_me(DLU_SUCCESS, "vert.spv and frag.spv officially created");

  VkShaderModule vert_shader_module = dlu_create_shader_module(app, cur_ld, shi_vert.bytes, shi_vert.byte_size);
  check_err(!vert_shader_module, app, wc, NULL)

  VkShaderModule frag_shader_module = dlu_create_shader_module(app, cur_ld, shi_frag.bytes, shi_frag.byte_size);
  check_err(!frag_shader_module, app, wc, vert_shader_module)

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
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = dlu_set_dynamic_state_info(2, dynamic_states);

  VkPipelineInputAssemblyStateCreateInfo input_assembly = dlu_set_input_assembly_state_info(0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

  VkPipelineRasterizationStateCreateInfo rasterizer = dlu_set_rasterization_state_info(
    VK_TRUE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
    VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
  );

  VkPipelineColorBlendAttachmentState color_blend_attachment = dlu_set_color_blend_attachment_state(
    VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
    0xf
  );

  float blend_const[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  VkPipelineColorBlendStateCreateInfo color_blending = dlu_set_color_blend_attachment_state_info(
    VK_FALSE, VK_LOGIC_OP_NO_OP, 1, &color_blend_attachment, blend_const
  );

  VkViewport viewport = dlu_set_view_port(0.0f, 0.0f, (float) extent2D.width, (float) extent2D.height, 0.0f, 1.0f);
  VkRect2D scissor = dlu_set_rect2D(0, 0, extent2D.width, extent2D.height);
  VkPipelineViewportStateCreateInfo view_port_info = dlu_set_view_port_state_info(1, &viewport, 1, &scissor);

  VkStencilOpState back = dlu_set_stencil_op_state(
    VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP,
    VK_COMPARE_OP_ALWAYS, 0, 0, 0
  );

  VkPipelineDepthStencilStateCreateInfo ds_info = dlu_set_depth_stencil_state(
    VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL,
    VK_FALSE, VK_FALSE, back, back, 0, 0
  );

  VkPipelineMultisampleStateCreateInfo multisampling = dlu_set_multisample_state_info(
    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, NULL, VK_FALSE, VK_FALSE
  );

  err = dlu_otba(DLU_GP_DATA_MEMS, app, cur_gpd, 1);
  check_err(!err, app, wc, NULL)

  err = dlu_create_graphics_pipelines(app, cur_gpd, ARR_LEN(shader_stages), shader_stages,
    &vertex_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, &ds_info, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  check_err(err, NULL, NULL, vert_shader_module)
  check_err(err, app, wc, frag_shader_module)

  dlu_log_me(DLU_SUCCESS, "Successfully created graphics pipeline");
  dlu_vk_destroy(DLU_DESTROY_VK_SHADER, app, cur_ld, frag_shader_module); frag_shader_module = VK_NULL_HANDLE;
  dlu_vk_destroy(DLU_DESTROY_VK_SHADER, app, cur_ld, vert_shader_module); vert_shader_module = VK_NULL_HANDLE;

  /* This also sets the descriptor count */
  err = dlu_otba(DLU_DESC_DATA_MEMS, app, cur_dd, NUM_DESCRIPTOR_SETS);
  check_err(!err, app, wc, NULL)

  err = dlu_create_desc_set_layout(app, cur_dd, 0, &desc_set_info[0]);
  check_err(err, app, wc, NULL)

  VkDescriptorPoolSize pool_size = dlu_set_desc_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NUM_DESCRIPTOR_SETS);
  err = dlu_create_desc_pool(app, cur_ld, cur_dd, 1, &pool_size, 0);
  check_err(err, app, wc, NULL)

  err = dlu_create_desc_sets(app, cur_dd);
  check_err(err, app, wc, NULL)

  VkClearValue clear_values[2];
  float float32[4] = {0.2f, 0.2f, 0.2f, 0.2f};
  int32_t int32[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  uint32_t uint32[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  clear_values[0] = dlu_set_clear_value(float32, int32, uint32, 0.0f, 0);
  clear_values[1] = dlu_set_clear_value(float32, int32, uint32, 1.0f, 1);

  err = dlu_exec_begin_cmd_buffs(app, cur_pool, cur_scd, 0, NULL);
  check_err(err, app, wc, NULL)

  /* Vertex buffer cannot be binded until we begin a renderpass */
  dlu_exec_begin_render_pass(app, cur_pool, cur_scd, cur_gpd, 0, 0, extent2D.width, extent2D.height, ARR_LEN(clear_values), clear_values, VK_SUBPASS_CONTENTS_INLINE);

  VkDescriptorBufferInfo buff_info; VkWriteDescriptorSet write;

  buff_info = dlu_set_desc_buff_info(app->buff_data[0].buff, offsets[1], sizeof(ubd.mvp));
  write = dlu_write_desc_set(app->desc_data[cur_dd].desc_set[0], 0, 0, NUM_DESCRIPTOR_SETS, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &buff_info, NULL);
  dlu_update_desc_sets(app->ld_data[cur_ld].device, NUM_DESCRIPTOR_SETS, &write, 0, NULL);

  dlu_bind_pipeline(app, cur_pool, cur_buff, cur_gpd, 0, VK_PIPELINE_BIND_POINT_GRAPHICS);
  dlu_bind_desc_sets(app, cur_pool, cur_buff, cur_gpd, cur_dd, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL);

  dlu_bind_vertex_buffs_to_cmd_buff(app, cur_pool, cur_buff, cur_bd, 0, offsets);
  dlu_exec_cmd_set_viewport(app, &viewport, cur_pool, cur_buff, 0, 1);
  dlu_exec_cmd_set_scissor(app, &scissor, cur_pool, cur_buff, 0, 1);
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

  s = suite_create("TestCube");

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

  sleep(4);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
