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

#define LUCUR_SPIRV_API
#include <lucom.h>

#include <shaderc/shaderc.h>

/** 
* Allows one to specify what type of shader they with to create SPIR-V bytes for
* key: VkShaderStageFlagBits, value: shaderc compilation type
*/
static const unsigned int shader_map_table[] = {
  [0x00000000] = shaderc_glsl_infer_from_source,
  [0x00000001] = shaderc_glsl_vertex_shader,
  [0x00000002] = shaderc_glsl_tess_control_shader,
  [0x00000004] = shaderc_glsl_tess_evaluation_shader,
  [0x00000008] = shaderc_glsl_geometry_shader,
  [0x00000010] = shaderc_glsl_fragment_shader,
  [0x00000020] = shaderc_glsl_compute_shader,
};

static void dlu_shaderc_release(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_compile_options_t options
) {
  if (options) shaderc_compile_options_release(options);
  if (result) shaderc_result_release(result);
  if (compiler) shaderc_compiler_release(compiler);
}

/* Returns GLSL shader source text after preprocessing */
dlu_shader_info dlu_preprocess_shader(
  unsigned int kind,
  const char *source,
  const char *input_file_name,
  const char *entry_point_name
) {

  dlu_shader_info shinfo = {0, NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compiler_t compiler = shaderc_compiler_initialize();
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compilation_result_t result = NULL;

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));
  shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_preprocessed_text(compiler, source, strlen(source), shader_map_table[kind],
                                                  input_file_name, entry_point_name, options);
  if (!result) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, NULL, options);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, result, options);
    return shinfo;
  }

  shinfo.result = result;
  shinfo.byte_size = shaderc_result_get_length(result);
  shinfo.bytes = (char *) shaderc_result_get_bytes(result);

  dlu_shaderc_release(compiler, NULL, options);

  return shinfo;
}

/* Compiles a shader to SPIR-V assembly. Returns the assembly text as a string. */
dlu_shader_info dlu_compile_to_assembly(
  unsigned int kind,
  const char *source,
  const char *input_file_name,
  const char *entry_point_name
) {

  dlu_shader_info shinfo = {NULL, NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compiler_t compiler = shaderc_compiler_initialize();
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compilation_result_t result = NULL;

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));
  shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv_assembly(compiler, source, strlen(source), shader_map_table[kind],
                                             input_file_name, entry_point_name, options);
  if (!result) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, NULL, options);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, result, options);
    return shinfo;
  }

  shinfo.result = result;
  shinfo.byte_size = shaderc_result_get_length(result);
  shinfo.bytes = (char *) shaderc_result_get_bytes(result);

  dlu_shaderc_release(compiler, NULL, options);

  return shinfo;
}

/* Compiles a shader to a SPIR-V binary */
dlu_shader_info dlu_compile_to_spirv(
  unsigned int kind,
  const char *source,
  const char *input_file_name,
  const char *entry_point_name
) {

  dlu_shader_info shinfo = {NULL, NULL, 0};

  const char *name = "MY_DEFINE";
  const char *value = "1";

  shaderc_compiler_t compiler = shaderc_compiler_initialize();
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compilation_result_t result = NULL;

  shaderc_compile_options_add_macro_definition(options, name, strlen(name), value, strlen(value));
  shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

  result = shaderc_compile_into_spv(compiler, source, strlen(source), shader_map_table[kind],
                                    input_file_name, entry_point_name, options);
  if (!result) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, NULL, options);
    return shinfo;
  }

  if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
    dlu_log_me(DLU_DANGER, "[x] %s", shaderc_result_get_error_message(result));
    dlu_shaderc_release(compiler, result, options);
    return shinfo;
  }

  shinfo.result = result;
  shinfo.byte_size = shaderc_result_get_length(result);
  shinfo.bytes = (char *) shaderc_result_get_bytes(result);

  /* Results are released in dlu_create_shader_module */
  dlu_shaderc_release(compiler, NULL, options);

  return shinfo;
}

void dlu_freeup_spriv_bytes(dlu_spirv_type type, void *res) {
  switch (type) {
    case DLU_UTILS_FILE_SPRIV: free(res); break;
    case DLU_LIB_SHADERC_SPRIV: shaderc_result_release(res); break;
    default: break;
  }
}
