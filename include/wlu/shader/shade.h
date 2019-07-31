#ifndef SHADE_H
#define SHADE_H

#include <shaderc/shaderc.h>

typedef struct wlu_shader_info {
  const char *bytes;
  long byte_size;
} wlu_shader_info;

wlu_shader_info wlu_preprocess_shader(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name
);

wlu_shader_info wlu_compile_to_assembly(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
);

wlu_shader_info wlu_compile_to_spirv(
  shaderc_compiler_t compiler,
  shaderc_compilation_result_t result,
  shaderc_shader_kind kind,
  const char *source,
  const char *source_name,
  const char *entry_point_name,
  bool optimize
);

#endif
