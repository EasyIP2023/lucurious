#include <lucom.h>

#include <wlu/shader/shade.h>
#include <wlu/utils/log.h>

/* Returns GLSL shader source text after preprocessing */
const char *wlu_preprocess_shader(shaderc_compiler_t compiler,
                                  shaderc_compilation_result_t result,
                                  shaderc_shader_kind kind,
                                  const char *source_name,
                                  const char *source,
                                  const char *entry_point_name) {

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  result = shaderc_compile_into_preprocessed_text(
           compiler, source_name, strlen(source_name),
           kind, source, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_preprocessed_text failed, ERROR code: %d", result);
    return NULL;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return NULL;
  }

  shaderc_compile_options_release(options);

  return shaderc_result_get_bytes(result);
}

/* Compiles a shader to SPIR-V assembly. Returns the assembly text as a string. */
const char *wlu_compile_to_assembly(shaderc_compiler_t compiler,
                                    shaderc_compilation_result_t result,
                                    shaderc_shader_kind kind,
                                    const char *source_name,
                                    const char *source,
                                    const char *entry_point_name,
                                    bool optimize) {

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  if (optimize)
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv_assembly(
           compiler, source_name, strlen(source_name),
           kind, source, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_spv_assembly failed, ERROR code: %d", result);
    return NULL;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return NULL;
  }

  shaderc_compile_options_release(options);

  return shaderc_result_get_bytes(result);
}


/* Compiles a shader to a SPIR-V binary */
const char *wlu_compile_to_spirv(shaderc_compiler_t compiler,
                                 shaderc_compilation_result_t result,
                                 shaderc_shader_kind kind,
                                 const char *source_name,
                                 const char *source,
                                 const char *entry_point_name,
                                 bool optimize) {

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compile_options_t options = shaderc_compile_options_initialize();

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));

  if (optimize)
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv(
           compiler, source_name, strlen(source_name),
           kind, source, entry_point_name, options);

  if (!result) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x] shaderc_compile_into_spv failed, ERROR code: %d", result);
    return NULL;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    shaderc_compile_options_release(options);
    wlu_log_me(WLU_DANGER, "[x]\n%s", shaderc_result_get_error_message(result));
    return NULL;
  }

  shaderc_compile_options_release(options);

  return shaderc_result_get_bytes(result);
}
