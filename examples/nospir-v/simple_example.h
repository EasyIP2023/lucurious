#ifndef SIMPLE_EXAMPLE_H
#define SIMPLE_EXAMPLE_H

const char *device_extensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char *instance_extensions[] = {
  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  VK_KHR_SURFACE_EXTENSION_NAME,
  VK_KHR_DISPLAY_EXTENSION_NAME
};

const char shader_frag_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) in vec3 fragColor;\n"
  "layout(location = 0) out vec4 outColor;\n"
  "void main() { outColor = vec4(fragColor, 1.0); }";

const char shader_vert_src[] =
  "#version 450\n"
  "#extension GL_ARB_separate_shader_objects : enable\n"
  "layout(location = 0) out vec3 fragColor;\n"
  "vec2 positions[3] = vec2[](\n"
  "\tvec2(0.0, -0.5),\n"
  "\tvec2(0.5, 0.5),\n"
  "\tvec2(-0.5, 0.5)\n"
  ");\n"
  "vec3 colors[3] = vec3[](\n"
  "\tvec3(1.0, 0.0, 0.0),\n"
  "\tvec3(0.0, 1.0, 0.0),\n"
  "\tvec3(0.0, 0.0, 1.0)\n"
  ");\n"
  "void main() {\n"
  "\tgl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);\n"
  "\tfragColor = colors[gl_VertexIndex];\n"
  "}";

#endif
