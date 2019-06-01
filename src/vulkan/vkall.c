#include <vlucur/vkall.h>
#include <vlucur/errors.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <vulkan/vulkan.h>

#include <cglm/call.h>

const int WIDTH = 800;
const int HEIGHT = 600;

/* All of the useful standard validation is
  bundled into a layer included in the SDK */
const char *validation_layers = { "VK_LAYER_KHRONOS_validation" };

#define NDEBUG // Defined here becuase I don't have the vulkan sdk installed

#ifdef NDEBUG
  const bool enable_validation_layers = false;
#else
  const bool enable_validation_layers = true;
#endif

struct queue_family_indices {
  int graphics_family;
};

struct vkcomp {
  /* Connection between application and the Vulkan library */
  VkInstance instance;

  VkExtensionProperties *vkprops;
  VkLayerProperties *available_layers;

  VkDebugUtilsMessengerEXT debug_messenger;

  /* To get device properties like the name, type and supported Vulkan version */
  VkPhysicalDeviceProperties device_properties;
  /* For optional features like texture compression,
    64 bit floats and multi viewport rendering */
  VkPhysicalDeviceFeatures device_features;
  VkPhysicalDevice physical_device;
  VkPhysicalDevice *devices;
  uint32_t device_count;

  VkQueueFamilyProperties *queue_families;
  uint32_t queue_family_count;

  struct queue_family_indices indices;

  VkDevice device; // logical device
  VkQueue graphics_queue;
};

bool check_validation_layer_support(vkcomp *app) {
  VkResult err;
  uint32_t layer_count;
  err = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
  assert(!err);

  app->available_layers = calloc(sizeof(VkLayerProperties), layer_count * sizeof(VkLayerProperties));
  assert(app->available_layers != NULL);

  err = vkEnumerateInstanceLayerProperties(&layer_count, app->available_layers);
  assert(!err);

  bool layer_found = false;

  for (uint32_t j = 0; j < layer_count; j++) {
    if (!strcmp(&validation_layers[0], app->available_layers[j].layerName)) {
      layer_found = true;
      break;
    }
  }

  return (layer_found) ? true : false;
}

void create_instance(vkcomp *app) {
  if (enable_validation_layers && !check_validation_layer_support(app)) {
    perror("[x] validation layers requested, but not available!\n");
    return;
  }

  VkResult err;

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

  if (enable_validation_layers) {
    create_info.enabledLayerCount = (uint32_t) (sizeof(validation_layers));
    create_info.ppEnabledLayerNames = &validation_layers;
  } else {
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledExtensionNames = &validation_layers;
  }

  /* Create the instance */
  err = vkCreateInstance(&create_info, NULL, &app->instance);

  if (err != VK_SUCCESS) {
    perror("[x] failed to created instance");
    return;
  }

  uint32_t extension_count = 0;
  err = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
  assert(!err);

  if (extension_count > 0) {

    app->vkprops = calloc(sizeof(VkExtensionProperties), extension_count * sizeof(VkExtensionProperties));
    assert(app->vkprops != NULL);

    err = vkEnumerateInstanceExtensionProperties(NULL, &extension_count, app->vkprops);
    assert(!err);

    printf("Instance created\navailable extesions: %d\n", extension_count);

    for (uint32_t i = 0; i < extension_count; i++)
      printf("%s\n", app->vkprops[i].extensionName);

  }
}

void setup_debug_messenger(vkcomp *app) {
  if (!enable_validation_layers) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

  /* specify all the types of severities you
    would like your callback to be called for */
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

  /* filter which types of messages your callback is
  notified about. all types are enabled here */
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
  create_info.pUserData = NULL; // Optional

  if (CreateDebugUtilsMessengerEXT(app->instance, &create_info, NULL, &app->debug_messenger) != VK_SUCCESS) {
    perror("[x] failed to set up debug messenger!");
    return;
  }
}

/*
find which queue families are supported by the device and which
one of these supports the commands that we want to use
*/
bool find_queue_families(vkcomp *app, VkPhysicalDevice device) {
  bool ret = false;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, NULL);
  app->queue_families = calloc(sizeof(VkQueueFamilyProperties), app->queue_family_count * sizeof(VkQueueFamilyProperties));
  assert(app->queue_families != NULL);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, app->queue_families);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT        ||
        app->queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ||
        app->queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT      ||
        app->queue_families[i].queueFlags & VK_QUEUE_FLAG_BITS_MAX_ENUM)
        app->indices.graphics_family = i;

    if (app->indices.graphics_family != -1) {
      ret = true;
      break;
    }
  }

  return ret;
}

bool is_device_suitable(vkcomp *app, VkPhysicalDevice device) {

  /* Query device properties */
  vkGetPhysicalDeviceProperties(device, &app->device_properties);
  vkGetPhysicalDeviceFeatures(device, &app->device_features);

  return find_queue_families(app, device) &&
        ((app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) &&
          app->device_features.geometryShader);
}

/* After selecting a physical device to use.
  Set up a logical device to interface with it */
void create_logical_device(vkcomp *app) {
  VkDeviceQueueCreateInfo queue_create_info = {};

  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = app->indices.graphics_family;
  queue_create_info.queueCount = app->queue_family_count;

  float queue_priority = 1.0f;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.pQueueCreateInfos = &queue_create_info;
  create_info.queueCreateInfoCount = app->queue_family_count;
  create_info.pEnabledFeatures = &app->device_features;
  create_info.enabledExtensionCount = 0;

  if (enable_validation_layers) {
    create_info.enabledLayerCount = (uint32_t) (sizeof(validation_layers));
    create_info.ppEnabledLayerNames = &validation_layers;
  } else {
    create_info.enabledLayerCount = 0;
  }

  /* Create logic device */
  if (vkCreateDevice(app->physical_device, &create_info, NULL, &app->device) != VK_SUCCESS) {
    perror("[x] failed to create logical device!");
    return;
  }

  vkGetDeviceQueue(app->device, app->indices.graphics_family, 0, &app->graphics_queue);
}

void pick_graphics_device(vkcomp *app) {
  VkResult err;
  err = vkEnumeratePhysicalDevices(app->instance, &app->device_count, NULL);
  assert(err != VK_ERROR_OUT_OF_HOST_MEMORY   ||
         err != VK_ERROR_OUT_OF_DEVICE_MEMORY ||
         err != VK_ERROR_INITIALIZATION_FAILED);


  if (app->device_count == 0) {
    perror("[x] failed to find GPUs with Vulkan support!");
    return;
  }

  app->devices = calloc(sizeof(VkPhysicalDevice), app->device_count * sizeof(VkPhysicalDevice));
  assert(app->devices != NULL);

  err = vkEnumeratePhysicalDevices(app->instance, &app->device_count, app->devices);
  assert(err != VK_ERROR_OUT_OF_HOST_MEMORY   ||
         err != VK_ERROR_OUT_OF_DEVICE_MEMORY ||
         err != VK_ERROR_INITIALIZATION_FAILED);

  /* get physical device */
  for (uint32_t i = 0; i < app->device_count; i++) {
    if (is_device_suitable(app, app->devices[i])) {
      app->physical_device = app->devices[i];
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    perror("[x] failed to find a suitable GPU!");
    return;
  }

  /* Query device properties */
  vkGetPhysicalDeviceProperties(app->physical_device, &app->device_properties);
  printf("physical_device found: %s\n", app->device_properties.deviceName);
}

void init_vulkan(vkcomp *app) {
  create_instance(app);
  setup_debug_messenger(app);
  pick_graphics_device(app);
  create_logical_device(app);
}

void reset_values(vkcomp *app) {
  app->instance = 0;
  app->vkprops = NULL;
  app->available_layers = NULL;
  app->debug_messenger = VK_NULL_HANDLE;
  //app->device_properties;
  //app->device_features;
  /* this gets destroyed when vkInstance is destroyed */
  app->physical_device = VK_NULL_HANDLE;
  app->devices = VK_NULL_HANDLE;
  app->device_count = 0;
  app->queue_families = NULL;
  app->queue_family_count = 0;
  app->indices.graphics_family = -1;
  app->device = VK_FALSE;
  app->graphics_queue = VK_FALSE;
}

void cleanup(vkcomp *app) {
  vkDeviceWaitIdle(app->device);
  vkDestroyDevice(app->device, NULL);

  if (enable_validation_layers)
    DestroyDebugUtilsMessengerEXT(app->instance, app->debug_messenger, NULL);

  free(app->vkprops);
  free(app->available_layers);
  free(app->devices);
  free(app->queue_families);

  vkDestroyInstance(app->instance, NULL);

  reset_values(app);

  free(app);
  app = NULL;
}

vkcomp *create_app(size_t init_value) {
  vkcomp *app = NULL;
  app = (vkcomp*) calloc(sizeof(vkcomp), init_value * sizeof(vkcomp));
  assert(app != NULL);
  return app;
}
