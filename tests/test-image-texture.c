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
#define LUCUR_WAYLAND_API
#define LUCUR_WAYLAND_CLIENT_API
#define LUCUR_SPIRV_API
#define LUCUR_CLOCK_API
#define CLOCK_MONOTONIC
#include <lucom.h>

#include "test-extras.h"
#include "test-shade.h"

#define WIDTH 800
#define HEIGHT 600
#define MAX_FRAMES 2

char *concat(char *fmt, ...);

static wlu_otma_mems ma = {
  .vkcomp_cnt = 10, .wclient_cnt = 10, .desc_cnt = 10,
  .gp_cnt = 10, .si_cnt = 15, .scd_cnt = 10, .gpd_cnt = 10,
  .cmdd_cnt = 10, .bd_cnt = 10, .td_cnt = 10
};

/* Be sure to make struct binary compatible with shader variable */
struct uniform_block_data {
  mat4 model;
  mat4 view;
  mat4 proj;
};

static bool init_buffs(vkcomp *app) {
  bool err;

  err = wlu_otba(WLU_BUFF_DATA, app, INDEX_IGNORE, 10);
  if (err) return err;

  err = wlu_otba(WLU_SC_DATA, app, INDEX_IGNORE, 1);
  if (err) return err;

  err = wlu_otba(WLU_GP_DATA, app, INDEX_IGNORE, 1);
  if (err) return err;

  err = wlu_otba(WLU_CMD_DATA, app, INDEX_IGNORE, 1);
  if (err) return err;

  err = wlu_otba(WLU_DESC_DATA, app, INDEX_IGNORE, 1);
  if (err) return err;

  err = wlu_otba(WLU_TEXT_DATA, app, INDEX_IGNORE, 1);
  if (err) return err;

  return err;
}

START_TEST(test_vulkan_image_texture) {
  VkResult err;

  if (!wlu_otma(WLU_LARGE_BLOCK_PRIV, ma)) ck_abort_msg(NULL);

  wclient *wc = wlu_init_wc();
  check_err(!wc, NULL, NULL, NULL)

  vkcomp *app = wlu_init_vk();
  check_err(!app, NULL, wc, NULL)

  err = init_buffs(app);
  check_err(err, app, wc, NULL)

  err = wlu_create_instance(app, "Image Texture", "No Engine", 1, enabled_validation_layers, 4, instance_extensions);
  check_err(err, app, wc, NULL)

  err = wlu_set_debug_message(app);
  check_err(err, app, wc, NULL)

  check_err(!wlu_create_client(wc), app, wc, NULL)

  /* initialize vulkan app surface */
  err = wlu_create_vkwayland_surfaceKHR(app, wc->display, wc->surface);
  check_err(err, app, wc, NULL)

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = wlu_create_physical_device(app, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, &device_props, &device_feats);
  check_err(err, app, wc, NULL)

  err = wlu_create_queue_families(app, VK_QUEUE_GRAPHICS_BIT);
  check_err(err, app, wc, NULL)

  device_feats.samplerAnisotropy = VK_TRUE;
  err = wlu_create_logical_device(app, &device_feats, 1, 1, enabled_validation_layers, 1, device_extensions);
  check_err(err, app, wc, NULL)

  VkSurfaceCapabilitiesKHR capabilities = wlu_get_physical_device_surface_capabilities(app);
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

  VkExtent2D extent = wlu_choose_swap_extent(capabilities, WIDTH, HEIGHT);
  check_err(extent.width == UINT32_MAX, app, wc, NULL)

  uint32_t cur_scd = 0, cur_pool = 0, cur_gpd = 0, cur_bd = 0, cur_cmdd = 0, cur_dd = 0;
  err = wlu_otba(WLU_SC_DATA_MEMS, app, cur_scd, capabilities.minImageCount);
  check_err(err, app, wc, NULL)

  err = wlu_create_swap_chain(app, cur_scd, capabilities, surface_fmt, pres_mode, extent.width, extent.height);
  check_err(err, app, wc, NULL)

  err = wlu_create_cmd_pool(app, cur_scd, cur_cmdd, app->indices.graphics_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  check_err(err, app, wc, NULL)

  err = wlu_create_cmd_buffs(app, cur_pool, cur_scd, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  check_err(err, app, wc, NULL)

  VkComponentMapping comp_map = wlu_set_component_mapping(VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A);
  VkImageSubresourceRange img_sub_rr = wlu_set_image_sub_resource_range(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
  VkImageViewCreateInfo img_view_info = wlu_set_image_view_info(0, VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_2D, surface_fmt.format, comp_map, img_sub_rr);

  /* Create Image Views. So that the application can access a VkImage resource */
  err = wlu_create_image_views(WLU_SC_IMAGE_VIEWS, app, cur_scd, &img_view_info);
  check_err(err, app, wc, NULL)

  /* This is where creation of the graphics pipeline begins */
  err = wlu_create_syncs(app, cur_scd);
  check_err(err, app, wc, NULL)

  /* Starting point for render pass creation */
  VkAttachmentDescription color_attachment = wlu_set_attachment_desc(surface_fmt.format,
    VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  );

  VkAttachmentReference color_attachment_ref = wlu_set_attachment_ref(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  VkSubpassDescription subpass = wlu_set_subpass_desc(VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &color_attachment_ref, NULL, NULL, 0, NULL);

  VkSubpassDependency subdep = wlu_set_subpass_dep(
    VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0
  );

  err = wlu_create_render_pass(app, cur_gpd, 1, &color_attachment, 1, &subpass, 1, &subdep);
  check_err(err, app, wc, NULL)

  wlu_log_me(WLU_SUCCESS, "Successfully created render pass");
  /* ending point for render pass creation */

  VkImageView vkimg_attach[1];
  err = wlu_create_framebuffers(app, cur_scd, cur_gpd, 1, vkimg_attach, extent.width, extent.height, 1);
  check_err(err, app, wc, NULL)

  err = wlu_create_pipeline_cache(app, 0, NULL);
  check_err(err, app, wc, NULL)

  /* Start of image loader */
  void *pixels = NULL; VkDeviceSize img_size; VkExtent3D img_extent;
  char *picture = concat("%s/tests/%s", WLU_DIR_SRC, "textures/water_train.jpg"); 
 
  err = wlu_load_texture_image(&img_extent, &pixels, picture); 
  check_err(err, app, wc, NULL)
  img_extent.depth = 1;

  wlu_log_me(WLU_SUCCESS, "textures/water_train.jpg successfully loaded", picture);
  free(picture); picture = NULL;
  /* End of image loader */

  img_size = img_extent.width * img_extent.height * 4;

  /**
  * The buffer is a staging host visible memory buffer. That can be map.
  * It's usable as a transfer source so that we can copy it to an image later on
  */
  err = wlu_create_vk_buffer(app, cur_bd, img_size, 0, 
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 's',
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, pixels);
  check_err(err, app, wc, NULL)
  cur_bd++;

  /* Now that the image has been moved into CPU visible momory the original pixels are no longer needed */
  wlu_freeup_pixels(pixels); pixels = NULL;

  VkImageCreateInfo img_info = wlu_set_image_info(0, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, img_extent, 1, 1,
    VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_IMAGE_LAYOUT_UNDEFINED
  );

  uint32_t cur_tex = 0;
  err = wlu_create_texture_image(app, cur_tex, &img_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  check_err(err, app, wc, NULL)

  VkImageMemoryBarrier barrier = wlu_set_image_mem_barrier(0, VK_ACCESS_TRANSFER_WRITE_BIT,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED,
    VK_QUEUE_FAMILY_IGNORED, app->text_data[cur_tex].image, img_sub_rr
  );

  /* Using image memory barrier to perform layout transitions */
  err = wlu_exec_pipeline_barrier(app, cur_pool, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier
  );
  check_err(err, app, wc, NULL)

  VkOffset3D offset3D = {0, 0, 0};
  VkImageSubresourceLayers img_sub_rl = wlu_set_image_sub_resource_layers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1);

  /* Specify what part of the buffer is copied to which part of the image */
  VkBufferImageCopy region = wlu_set_buff_image_copy(0, 0, 0, img_sub_rl, offset3D, img_extent);

  /* cur_bd: must be a valid VkBuffer that contains your image pixels */
  err = wlu_exec_copy_buff_to_image(app, cur_pool, cur_bd-1, cur_tex, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  check_err(err, app, wc, NULL)

  /**
  * Allows for in shader sampling of a texture image,
  * This is the last transition to run, to prepare for shader access
  */
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  err = wlu_exec_pipeline_barrier(app, cur_pool, VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier
  );
  check_err(err, app, wc, NULL)

  /* Destroy staging buffer as it is no longer needed */
  wlu_vk_destroy(WLU_DESTROY_VK_BUFFER, app, app->buff_data[cur_bd-1].buff); app->buff_data[cur_bd-1].buff = VK_NULL_HANDLE;
  wlu_vk_destroy(WLU_DESTROY_VK_MEMORY, app, app->buff_data[cur_bd-1].mem); app->buff_data[cur_bd-1].mem = VK_NULL_HANDLE;

  /* Create Image View for texture image. So that application can access a VkImage resource */
  img_view_info.format = VK_FORMAT_R8G8B8A8_UNORM; /* VK_COMPONENT_SWIZZLE_IDENTITY defined as zero */
  img_view_info.components = wlu_set_component_mapping(0, 0, 0, 0);

  err = wlu_create_image_views(WLU_TEXT_IMAGE_VIEWS, app, cur_tex, &img_view_info);
  check_err(err, app, wc, NULL)

  VkSamplerCreateInfo sampler = wlu_set_sampler_info(0, VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, VK_SAMPLER_MIPMAP_MODE_LINEAR,
    VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 16.0f, VK_TRUE, VK_FALSE,
    VK_COMPARE_OP_ALWAYS, 0.0f, 0.0f, VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_FALSE
  );

  err = wlu_create_texture_sampler(app, cur_tex, &sampler);
  check_err(err, app, wc, NULL)

  /* 0 is the binding. The # of is bytes there is between successive structs */
  VkVertexInputBindingDescription vi_binding = wlu_set_vertex_input_binding_desc(0, sizeof(vertex_text_2D), VK_VERTEX_INPUT_RATE_VERTEX);

  VkVertexInputAttributeDescription vi_attribs[3];
  vi_attribs[0] = wlu_set_vertex_input_attrib_desc(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_text_2D, pos));
  vi_attribs[1] = wlu_set_vertex_input_attrib_desc(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_text_2D, color));
  vi_attribs[2] = wlu_set_vertex_input_attrib_desc(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_text_2D, tex_coord));

  VkPipelineVertexInputStateCreateInfo vertex_input_info = wlu_set_vertex_input_state_info(1, &vi_binding, 3, vi_attribs);

  /* This also sets the descriptor count */
  err = wlu_otba(WLU_DESC_DATA_MEMS, app, cur_dd, app->sc_data[cur_scd].sic);
  check_err(err, app, wc, NULL)

  VkDescriptorSetLayoutBinding bindings[2];
  bindings[0] = wlu_set_desc_set_layout_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, NULL);
  bindings[1] = wlu_set_desc_set_layout_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL);

  VkDescriptorSetLayoutCreateInfo desc_set_info = wlu_set_desc_set_layout_info(0, 2, bindings);

  /* Using mutiple layouts for all VkDescriptorSetLayout objects */
  err = wlu_create_desc_set_layouts(app, cur_dd, &desc_set_info);
  check_err(err, app, wc, NULL)

  err = wlu_create_pipeline_layout(app, cur_gpd, cur_dd, 0, NULL);
  check_err(err, app, wc, NULL)

  wlu_log_me(WLU_INFO, "Start of shader creation");
  wlu_log_me(WLU_WARNING, "Compiling the fragment shader code to spirv bytes");
  wlu_shader_info shi_frag = wlu_compile_to_spirv(VK_SHADER_STAGE_FRAGMENT_BIT, text_map_frag_src, "frag.spv", "main");
  check_err(!shi_frag.bytes, app, wc, NULL)

  wlu_log_me(WLU_WARNING, "Compiling the vertex shader code into spirv bytes");
  wlu_shader_info shi_vert = wlu_compile_to_spirv(VK_SHADER_STAGE_VERTEX_BIT, text_map_vert_src, "vert.spv", "main");
  check_err(!shi_vert.bytes, app, wc, NULL)

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, shi_frag.bytes, shi_frag.byte_size);
  check_err(!frag_shader_module, app, wc, NULL)

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, shi_vert.bytes, shi_vert.byte_size);
  check_err(!vert_shader_module, app, wc, frag_shader_module)

  wlu_freeup_spriv_bytes(WLU_LIB_SHADERC_SPRIV, shi_vert.result);
  wlu_freeup_spriv_bytes(WLU_LIB_SHADERC_SPRIV, shi_frag.result);
  wlu_log_me(WLU_INFO, "End of shader creation");

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = wlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = wlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo shader_stages[2] = { vert_shader_stage_info, frag_shader_stage_info };
  VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

  VkPipelineDynamicStateCreateInfo dynamic_state = wlu_set_dynamic_state_info(2, dynamic_states);

  VkPipelineInputAssemblyStateCreateInfo input_assembly = wlu_set_input_assembly_state_info(
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
  );

  VkViewport viewport = wlu_set_view_port(0.0f, 0.0f, (float) extent.width, (float) extent.height, 0.0f, 1.0f);
  VkRect2D scissor = wlu_set_rect2D(0, 0, extent.width, extent.height);
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

  err = wlu_otba(WLU_GP_DATA_MEMS, app, cur_gpd, 1);
  check_err(err, app, wc, NULL)

  err = wlu_create_graphics_pipelines(app, cur_gpd, 2, shader_stages,
    &vertex_input_info, &input_assembly, VK_NULL_HANDLE, &view_port_info,
    &rasterizer, &multisampling, VK_NULL_HANDLE, &color_blending,
    &dynamic_state, 0, VK_NULL_HANDLE, UINT32_MAX
  );
  check_err(err, NULL, NULL, vert_shader_module)
  check_err(err, app, wc, frag_shader_module)

  wlu_log_me(WLU_SUCCESS, "graphics pipeline creation successfull");
  wlu_vk_destroy(WLU_DESTROY_VK_SHADER, app, frag_shader_module); frag_shader_module = VK_NULL_HANDLE;
  wlu_vk_destroy(WLU_DESTROY_VK_SHADER, app, vert_shader_module); vert_shader_module = VK_NULL_HANDLE;
  /* Ending setup for graphics pipeline */

  /* Start of staging buffer for vertex */
  vertex_text_2D vertices[4] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
  };

  VkDeviceSize vsize = sizeof(vertices);
  const uint32_t vertex_count = vsize / sizeof(vertex_2D);

  for (uint32_t i = 0; i < vertex_count; i++) {
    wlu_print_vector(WLU_VEC2, &vertices[i].pos);
    wlu_print_vector(WLU_VEC3, &vertices[i].color);
  }

  /**
  * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
  * The VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT communicates that the memory
  * should be mapped so that the CPU (host) can access it.
  * The VK_MEMORY_PROPERTY_HOST_COHERENT_BIT requests that the
  * writes to the memory by the host are visible to the device
  * (and vice-versa) without the need to flush memory caches.
  */
  err = wlu_create_vk_buffer(
    app, cur_bd, vsize, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 's',
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, vertices);
  check_err(err, app, wc, NULL)
  cur_bd++;
  /* End of staging buffer for vertex */

  /* Start of vertex buffer */
  err = wlu_create_vk_buffer(
    app, cur_bd, vsize, 0,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'v',
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, NULL);
  check_err(err, app, wc, NULL)
  cur_bd++;

  /* Used to show functionality */
  err = wlu_exec_copy_buffer(app, cur_pool, cur_bd-2, cur_bd-1, 0, 0, vsize);
  check_err(err, app, wc, NULL)
  /* End of vertex buffer */

  /* Destroy staging buffer as it is no longer needed */
  wlu_vk_destroy(WLU_DESTROY_VK_BUFFER, app, app->buff_data[cur_bd-2].buff); app->buff_data[cur_bd-2].buff = VK_NULL_HANDLE;
  wlu_vk_destroy(WLU_DESTROY_VK_MEMORY, app, app->buff_data[cur_bd-2].mem); app->buff_data[cur_bd-2].mem = VK_NULL_HANDLE;

  /* Start of staging buffer for index */
  VkDeviceSize isize = sizeof(indices);
  const uint32_t index_count = isize / sizeof(uint16_t); /* 2 bytes */
  err = wlu_create_vk_buffer(
    app, cur_bd, isize, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 's',
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, indices);
  check_err(err, app, wc, NULL)
  cur_bd++;
  /* End of staging buffer for index */

  /* Start of index buffer */
  err = wlu_create_vk_buffer(
    app, cur_bd, isize, 0,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'i',
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  check_err(err, app, wc, NULL)

  err = wlu_create_buff_mem_map(app, cur_bd, NULL);
  check_err(err, app, wc, NULL)
  cur_bd++;

  /* Used to show functionality */
  err = wlu_exec_copy_buffer(app, cur_pool, cur_bd-2, cur_bd-1, 0, 0, isize);
  check_err(err, app, wc, NULL)
  /* End of index buffer */

  /* Destroy staging buffer as it is no longer needed */
  wlu_vk_destroy(WLU_DESTROY_VK_BUFFER, app, app->buff_data[cur_bd-2].buff); app->buff_data[cur_bd-2].buff = VK_NULL_HANDLE;
  wlu_vk_destroy(WLU_DESTROY_VK_MEMORY, app, app->buff_data[cur_bd-2].mem); app->buff_data[cur_bd-2].mem = VK_NULL_HANDLE;

  /**
  * Now Creating uniform buffers
  * Passing u character here so that wlu_freeup_sc() function can
  * destroy uniform buffers allowing for easier swap chain recreation
  */
  for (uint32_t i = cur_bd; i < (cur_bd+app->sc_data[cur_scd].sic); i++) {
    err = wlu_create_vk_buffer(app, i, sizeof(struct uniform_block_data), 0,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, 'u',
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    check_err(err, app, wc, NULL)
    wlu_log_me(WLU_SUCCESS, "Just Allocated!!!");
    wlu_log_me(WLU_INFO, "app->buff_data[%d].name: %c", i, app->buff_data[i].name);
    wlu_log_me(WLU_INFO, "app->buff_data[%d].buff: %p - %p", i, &app->buff_data[i].buff, app->buff_data[i].buff);
  }
  /* Done creating uniform buffers */

  VkDescriptorPoolSize pool_sizes[2];
  pool_sizes[0] = wlu_set_desc_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, app->sc_data[cur_scd].sic);
  pool_sizes[1] = wlu_set_desc_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, app->sc_data[cur_scd].sic);

  err = wlu_create_desc_pool(app, cur_dd, 0, 2, pool_sizes);
  check_err(err, app, wc, NULL)

  err = wlu_create_desc_set(app, cur_dd);
  check_err(err, app, wc, NULL)

  /* set uniform buffer VKBufferInfos */
  VkDescriptorBufferInfo buff_info;
  VkWriteDescriptorSet writes[2];

  VkDescriptorImageInfo desc_img_info = {};
  desc_img_info.sampler = app->text_data[cur_tex].sampler;
  desc_img_info.imageView = app->text_data[cur_tex].view;
  desc_img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    buff_info = wlu_set_desc_buff_info(app->buff_data[i+cur_bd].buff, 0, VK_WHOLE_SIZE);
    writes[0] = wlu_write_desc_set(app->desc_data[cur_dd].desc_set[i], 0, 0, 1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &buff_info, NULL);
    writes[1] = wlu_write_desc_set(app->desc_data[cur_dd].desc_set[i], 1, 0, 1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &desc_img_info, NULL, NULL);
    wlu_update_desc_sets(app, 2, writes, 0, NULL);
  }

  wlu_log_me(WLU_SUCCESS, "ALL ALLOCATED BUFFERS");
  for (uint32_t i = 0; i < app->bdc; i++) {
    wlu_log_me(WLU_INFO, "app->buff_data[%d].name: %c", i, app->buff_data[i].name);
    wlu_log_me(WLU_INFO, "app->buff_data[%d].buff: %p - %p", i, &app->buff_data[i].buff, app->buff_data[i].buff);
  }

  float float32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  int32_t int32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  uint32_t uint32[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  VkClearValue clear_value = wlu_set_clear_value(float32, int32, uint32, 0.0f, 0);

  /* Set command buffers into recording state */
  err = wlu_exec_begin_cmd_buffs(app, cur_pool, cur_scd, 0, NULL);
  check_err(err, app, wc, NULL)

  /* Drawing will start when you begin a render pass */
  wlu_exec_begin_render_pass(app, cur_pool, cur_scd, cur_gpd, 0, 0, extent.width, extent.height, 2, &clear_value, VK_SUBPASS_CONTENTS_INLINE);

  const VkDeviceSize offsets[1] = {0}; /* Bind and draw in all available command buffers */
  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    wlu_cmd_set_viewport(app, &viewport, cur_pool, i, 0, 1);
    wlu_bind_pipeline(app, cur_pool, i, VK_PIPELINE_BIND_POINT_GRAPHICS, app->gp_data[cur_gpd].graphics_pipelines[0]);
    wlu_bind_vertex_buffs_to_cmd_buff(app, cur_pool, i, 0, 1, &app->buff_data[2].buff, offsets);
    wlu_bind_index_buff_to_cmd_buff(app, cur_pool, i, app->buff_data[4].buff, offsets[0], VK_INDEX_TYPE_UINT16);
    wlu_bind_desc_sets(app, cur_pool, i, cur_gpd, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, 1, &app->desc_data[cur_dd].desc_set[i], 0, NULL);
    wlu_cmd_draw_indexed(app, cur_pool, i, index_count, 1, 0, offsets[0], 0);
  }

  wlu_exec_stop_render_pass(app, cur_pool, cur_scd);
  err = wlu_exec_stop_cmd_buffs(app, cur_pool, cur_scd);
  check_err(err, app, wc, NULL)

  uint64_t time = 0, start = wlu_hrnst();
  uint32_t cur_frame = 0, img_index;

  struct uniform_block_data ubd;
  float convert = 1000000000.0f;
  float fovy = wlu_set_radian(45.0f), angle = wlu_set_radian(90.f);
  float hw = (float) extent.width / (float) extent.height;
  if (extent.width > extent.height) fovy *= hw;
  wlu_set_perspective(ubd.proj, fovy, hw, 0.1f, 10.0f);
  wlu_set_lookat(ubd.view, spin_eye, spin_center, spin_up);
  ubd.proj[1][1] *= -1; /* Invert Y-Coordinate */

  VkCommandBuffer cmd_buffs[app->sc_data[cur_scd].sic];
  VkSemaphore acquire_sems[MAX_FRAMES], render_sems[MAX_FRAMES];
  VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  for (uint32_t c = 0; c < 3000; c++) {
    /* set fence to signal state */
    err = wlu_vk_sync(WLU_VK_WAIT_RENDER_FENCE, app, cur_scd, cur_frame);
    check_err(err, app, wc, NULL)

    err = wlu_acquire_sc_image_index(app, cur_scd, cur_frame, &img_index);
    check_err(err, app, wc, NULL)

    cmd_buffs[img_index] = app->cmd_data[cur_cmdd].cmd_buffs[img_index];
    acquire_sems[cur_frame] = app->sc_data[cur_scd].syncs[cur_frame].sem.image;
    render_sems[cur_frame] = app->sc_data[cur_scd].syncs[cur_frame].sem.render;

    time = wlu_hrnst() - start;
    wlu_set_matrix(WLU_MAT4_IDENTITY, ubd.model, NULL);
    wlu_set_rotate(WLU_AXIS_Z, ubd.model, ((float) time / convert) * angle, spin_up);

    err = wlu_create_buff_mem_map(app, cur_bd+img_index, &ubd);
    check_err(err, app, wc, NULL)

    /* Check if a previous frame is using image */
    if (app->sc_data[cur_scd].syncs[img_index].fence.image) {
      err = wlu_vk_sync(WLU_VK_WAIT_IMAGE_FENCE, app, cur_scd, cur_frame);
      check_err(err, app, wc, NULL)
    }

    /* Mark the image as being in use by current frame */
    app->sc_data[cur_scd].syncs[img_index].fence.image = app->sc_data[cur_scd].syncs[cur_frame].fence.render;

    /* set fence to unsignal state */
    err = wlu_vk_sync(WLU_VK_RESET_RENDER_FENCE, app, cur_scd, cur_frame);
    check_err(err, app, wc, NULL)

    err = wlu_queue_graphics_queue(app, cur_scd, cur_frame, 1, &cmd_buffs[img_index], 1, &acquire_sems[cur_frame], &wait_stage, 1, &render_sems[cur_frame]);
    check_err(err, app, wc, NULL)

    err = wlu_queue_present_queue(app, 1, &render_sems[cur_frame], 1, &app->sc_data[cur_scd].swap_chain, &img_index, NULL);
    check_err(err, app, wc, NULL)

    cur_frame = (cur_frame + 1) % MAX_FRAMES;
  }

  FREEME(app, wc)
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestImageTexture");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_vulkan_image_texture);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());

  sleep(8);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
