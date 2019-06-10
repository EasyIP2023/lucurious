#include <lucom.h>
#include <vlucur/vkall.h>

/* All of the useful standard validation is
  bundled into a layer included in the SDK */
const char *validation_layers = { "VK_LAYER_KHRONOS_validation" };

void initial_values(struct vkcomp *app) {
  app->instance = 0;
  app->instance_layer_properties = NULL;
  app->instance_layer_count = 0;
}

VkResult get_instance_extension_properties(struct vkcomp *app, VkLayerProperties *prop) {
  VkResult res = VK_INCOMPLETE;
  VkExtensionProperties *instance_extension = NULL;
  uint32_t instance_extension_count = 0;

  do {
    res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extension) :
                  vkEnumerateInstanceExtensionProperties(prop->layerName, &instance_extension_count, NULL);
    if (res) return res;

    if (instance_extension_count == 0)
      return VK_SUCCESS;

    instance_extension = realloc(instance_extension, instance_extension_count * sizeof(VkExtensionProperties));
    assert(instance_extension != NULL);

    res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extension) :
                  vkEnumerateInstanceExtensionProperties(prop->layerName, &instance_extension_count, instance_extension);
  } while (res == VK_INCOMPLETE);

  if (app) {
    fprintf(stderr, "Instance created\navailable extesions: %d\n", instance_extension_count);

    for (uint32_t i = 0; i < instance_extension_count; i++)
      fprintf(stderr, "%s\n", instance_extension[i].extensionName);
  }

  free(instance_extension);
  instance_extension = NULL;

  return res;
}

/*
 * Gets all you're validation layers extensions
 * that you can later using pointer arithmetic cycle through
 */
VkResult check_validation_layer_support(struct vkcomp *app) {
  uint32_t layer_count = 0;
  VkLayerProperties *vk_props = NULL;
  VkResult res = VK_INCOMPLETE;

  do {
    res = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (res) return res;

    if (layer_count == 0) return VK_SUCCESS;

    vk_props = (VkLayerProperties *) realloc(vk_props, layer_count * sizeof(VkLayerProperties));
    assert(vk_props != NULL);

    res = vkEnumerateInstanceLayerProperties(&layer_count, vk_props);
  } while (res == VK_INCOMPLETE);

  if (app->instance_layer_properties == NULL)
    app->instance_layer_properties = (VkLayerProperties *) \
      calloc(sizeof(VkLayerProperties), layer_count * sizeof(VkLayerProperties));

  /* Gather the extension list for each instance layer. */
  for (uint32_t i = 0; i < layer_count; i++) {
    res = get_instance_extension_properties(NULL, &vk_props[i]);
    if (res) return res;

    /* must to do a deep copy of the structs information */
    memcpy(app->instance_layer_properties[app->instance_layer_count].layerName,
           vk_props[i].layerName, strlen(vk_props[i].layerName) + 1);
    memcpy(app->instance_layer_properties[app->instance_layer_count].description,
           vk_props[i].description, strlen(vk_props[i].description) + 1);
    app->instance_layer_properties[app->instance_layer_count].specVersion = vk_props[i].specVersion;
    app->instance_layer_properties[app->instance_layer_count].implementationVersion = vk_props[i].implementationVersion;
    app->instance_layer_count++;
  }

  free(vk_props);
  vk_props = NULL;

  return res;
}

VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name) {
  VkResult res;

  /* initialize the VkApplicationInfo structure */
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = engine_name;
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /* tells the Vulkan driver which global extensions
    and validation layers we want to use */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = 0;
  create_info.ppEnabledExtensionNames = NULL;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = NULL;

  /* Create the instance */
  res = vkCreateInstance(&create_info, NULL, &app->instance);
  switch(res) {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return res;
    case VK_ERROR_INITIALIZATION_FAILED:
      return res;
    case VK_ERROR_LAYER_NOT_PRESENT:
      return res;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return res;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      perror("[x] cannot find a compatible Vulkan ICD\n");
      return res;
    default:
      perror("[x] unknown error\n");
      return res;
  }

  res = get_instance_extension_properties(app, NULL);
  if (res) return res;

  return res;
}

void cleanup(struct vkcomp *app) {
  free(app->instance_layer_properties);
  vkDestroyInstance(app->instance, NULL);

  initial_values(app);
}
