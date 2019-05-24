#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <cglm/call.h>

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", NULL, NULL);

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
  printf("%d extensions supported\n", extensionCount);

  mat4 m;
  vec4 v, dest;

  glmc_mat4_mulv(m,v,dest);

  while(!glfwWindowShouldClose(window))
    glfwPollEvents();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
