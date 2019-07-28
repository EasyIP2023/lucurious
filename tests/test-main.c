#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/wclient/client.h>
#include <wlu/utils/errors.h>
#include <wlu/utils/log.h>
#include <wlu/shader/shade.h>
#include <wlu/vlucur/gp.h>

#include <signal.h>
#include <check.h>

#include "test-shade.h"

#define WIDTH 1920
#define HEIGHT 1080

void freesh(shaderc_compiler_t compiler, shaderc_compilation_result_t result) {
  shaderc_result_release(result);
  shaderc_compiler_release(compiler);
}

void freeme(vkcomp *app, wclient *wc) {
  wlu_freeup_vk(app);
  wlu_freeup_wc(wc);
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
    freeme(NULL, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_init_vk failed!!");
    ck_abort_msg(NULL);
  }

  /* Signal handler for this process */
  err = wlu_watch_me(SIGSEGV, 0, getpid(), app, wc);
  if (err) {
    freeme(app, wc);
    ck_abort_msg(NULL);
  }

  if (wlu_connect_client(wc)) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to connect client");
    ck_abort_msg(NULL);
  }

  err = wlu_set_global_layers(app);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] checking and setting validation layers failed");
    ck_abort_msg(NULL);
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create vulkan instance");
    ck_abort_msg(NULL);
  }

  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to connect to vulkan surfaceKHR");
    ck_abort_msg(NULL);
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to find physical device");
    ck_abort_msg(NULL);
  }

  err = wlu_create_logical_device(app);
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

  VkExtent2D extent = wlu_choose_swap_extent(capabilities, WIDTH, HEIGHT);
  if (extent.width == UINT32_MAX) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] choose_swap_extent failed, extent.width equals %d", extent.width);
    ck_abort_msg(NULL);
  }

  err = wlu_create_swap_chain(app, capabilities, surface_fmt, pres_mode, extent);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create swap chain");
    ck_abort_msg(NULL);
  }

  err = wlu_create_img_views(app, TWO_D_IMG);
  if (err) {
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create image views");
    ck_abort_msg(NULL);
  }

  /* This is where creation of the graphics pipeline begins */

  shaderc_compiler_t compiler = shaderc_compiler_initialize();
  shaderc_compilation_result_t result = 0;

  wlu_log_me(WLU_WARNING, "Compiling the frag spirv shader");
  const uint32_t *frag_spv = wlu_compile_to_spirv(compiler, result,
                             shaderc_glsl_vertex_shader, shader_frag_src,
                             "frag.spv", "main", false);
  if (!frag_spv) {
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  wlu_log_me(WLU_WARNING, "Compiling the vert spirv shader");
  const uint32_t *vert_spv = wlu_compile_to_spirv(compiler, result,
                             shaderc_glsl_vertex_shader, shader_vert_src,
                             "vert.spv", "main", false);
  if (!vert_spv) {
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] wlu_compile_to_spirv failed");
    ck_abort_msg(NULL);
  }

  VkShaderModule vert_shader_module = wlu_create_shader_module(app, vert_spv);
  if (vert_shader_module == NULL) {
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }

  VkShaderModule frag_shader_module = wlu_create_shader_module(app, frag_spv);
  if (frag_shader_module == NULL) {
    wlu_freeup_shader(app, vert_shader_module);
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to create shader module");
    ck_abort_msg(NULL);
  }
  /*
  VkPipelineShaderStageCreateInfo vert_shader_stage_info = wlu_set_shader_stage_info(
    vert_shader_module, "main", VK_SHADER_STAGE_VERTEX_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = wlu_set_shader_stage_info(
    frag_shader_module, "main", VK_SHADER_STAGE_FRAGMENT_BIT, NULL
  );

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info, frag_shader_stage_info
  };

  VkPipelineVertexInputStateCreateInfo vertext_input_info = wlu_set_vertext_input_state_info(
    0, NULL, 0, NULL
  );

  VkPipelineInputAssemblyStateCreateInfo input_assembly = wlu_set_input_assembly_state_info(
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
  );

  VkViewport viewport = wlu_set_view_port(0.0f, 0.0f, (float) extent.width, (float) extent.height, 0.0f, 1.0f);

  VkRect2D scissor = wlu_set_rect2D(0, 0, extent);

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

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = wlu_set_dynamic_state_info(2, dynamic_states);
  */

  err = wlu_create_pipeline_layout(app, 0, NULL, 0, NULL);
  if (err) {
    wlu_freeup_shader(app, frag_shader_module);
    wlu_freeup_shader(app, vert_shader_module);
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to run wayland client");
    ck_abort_msg(NULL);
  }

  /* Ending setup for graphics pipeline */
  if (wlu_run_client(wc)) {
    wlu_freeup_shader(app, frag_shader_module);
    wlu_freeup_shader(app, vert_shader_module);
    freesh(compiler, result);
    freeme(app, wc);
    wlu_log_me(WLU_DANGER, "[x] failed to run wayland client");
    ck_abort_msg(NULL);
  }

  wlu_freeup_shader(app, frag_shader_module);
  wlu_freeup_shader(app, vert_shader_module);
  freesh(compiler, result);
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
