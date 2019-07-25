#ifndef SHADE_H
#define SHADE_H

#include <shaderc/shaderc.h>

const char *wlu_preprocess_shader(shaderc_compiler_t compiler,
                                  shaderc_compilation_result_t result,
                                  shaderc_shader_kind kind,
                                  const char *source,
                                  const char *source_name,
                                  const char *entry_point_name);

const char *wlu_compile_to_assembly(shaderc_compiler_t compiler,
                                    shaderc_compilation_result_t result,
                                    shaderc_shader_kind kind,
                                    const char *source,
                                    const char *source_name,
                                    const char *entry_point_name,
                                    bool optimize);

const uint32_t *wlu_compile_to_spirv(shaderc_compiler_t compiler,
                                     shaderc_compilation_result_t result,
                                     shaderc_shader_kind kind,
                                     const char *source,
                                     const char *source_name,
                                     const char *entry_point_name,
                                     bool optimize);
#endif
