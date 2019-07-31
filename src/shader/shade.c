#include <lucom.h>

#include <wlu/shader/shade.h>
#include <wlu/utils/log.h>

/* Returns GLSL shader source text after preprocessing */
wlu_shader_info wlu_preprocess_shader(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name
) {

  wlu_shader_info shinfo = {NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  result = shaderc_compile_into_preprocessed_text(
           compiler, source, strlen(source), kind,
           source_name, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_preprocessed_text failed, ERROR code: %d", result);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return shinfo;
  }

  shaderc_compile_options_release(options);

  shinfo.bytes = shaderc_result_get_bytes(result);
  shinfo.byte_size = shaderc_result_get_length(result);

  return shinfo;
}

/* Compiles a shader to SPIR-V assembly. Returns the assembly text as a string. */
wlu_shader_info wlu_compile_to_assembly(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
) {

  wlu_shader_info shinfo = {NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  if (optimize)
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv_assembly(
           compiler, source, strlen(source), kind,
           source_name, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_spv_assembly failed, ERROR code: %d", result);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return shinfo;
  }

  shaderc_compile_options_release(options);

  shinfo.bytes = shaderc_result_get_bytes(result);
  shinfo.byte_size = shaderc_result_get_length(result);

  return shinfo;
}

/* Compiles a shader to a SPIR-V binary */
wlu_shader_info wlu_compile_to_spirv(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
) {

  wlu_shader_info shinfo = {NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  if (optimize)
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv(
           compiler, source, strlen(source), kind,
           source_name, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_spv failed, ERROR code: %d", result);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return shinfo;
  }

  shaderc_compile_options_release(options);

  shinfo.bytes = shaderc_result_get_bytes(result);
  shinfo.byte_size = shaderc_result_get_length(result);

  return shinfo;
}
