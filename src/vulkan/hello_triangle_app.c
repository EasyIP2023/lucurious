#include <vlucur/vlucur.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cglm/call.h>

const int WIDTH = 800;
const int HEIGHT = 600;

GLFWwindow *window = NULL;

/* Connection between application and the Vulkan library */
VkInstance instance;
VkExtensionProperties *vkprops;

void init_window() {
  if(!glfwInit()) return;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
  glfwMakeContextCurrent(window);
}

void main_loop() {
  printf("glfwWindow %d\n", glfwWindowShouldClose(window));

  // Still working on why window not showing up
  // Could be the wayland compositor I'm using
  return;
  while (!glfwWindowShouldClose(window))
    glfwPollEvents();
}

void create_instance() {

  VkApplicationInfo app_info = {};
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /* tells the Vulkan driver which global extensions
    and validation layers we want to use */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  /* glfw3 extesion to interface vulkan with the window system */
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = glfw_extensions;
  create_info.enabledLayerCount = 0;

  /* Create the instance */
  VkResult result = vkCreateInstance(&create_info, NULL, &instance);
  if (result != VK_SUCCESS) {
    perror("[x] failed to created instance");
    return;
  }


  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

  vkprops = calloc(sizeof(VkExtensionProperties), extension_count * sizeof(VkExtensionProperties));
  assert(vkprops != NULL);

  vkEnumerateInstanceExtensionProperties(NULL, &extension_count, vkprops);

  printf("Instance created\navailable extesions: %d\n",extension_count);

  for (unsigned int i = 0; i < extension_count; i++)
    printf("%s\n", vkprops[i].extensionName);

}

void init_vulkan() {
  create_instance();
}

void cleanup() {
  free(vkprops);
  vkprops = NULL;
  vkDestroyInstance(instance, NULL);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void run() {
  init_window();
  init_vulkan();
  main_loop();
  cleanup();
}
