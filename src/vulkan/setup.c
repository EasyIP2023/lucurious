#include <vlucur/vkall.h>

/* All of the useful standard validation is
  bundled into a layer included in the SDK */
const char *validation_layers = { "VK_LAYER_KHRONOS_validation" };

void initialize_vulkan_values(struct vkcomp *app) {
  app->instance = 0;
  app->surface = VK_NULL_HANDLE;
  app->instance_layer_properties = NULL;
  app->instance_layer_count = 0;
  app->physical_device = VK_NULL_HANDLE;
  app->devices = VK_NULL_HANDLE;
  app->device_count = 0;
  app->queue_families = NULL;
  app->queue_family_count = 0;
  app->indices.graphics_family = -1;
  app->device = VK_FALSE;
  app->graphics_queue = VK_FALSE;
  app->queue_create_infos = NULL;
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
    fprintf(stdout, "\nINSTANCE CREATED\n\nAVAILABLE EXTESIONS: %d\n", instance_extension_count);

    for (uint32_t i = 0; i < instance_extension_count; i++)
      fprintf(stdout, "%s\n", instance_extension[i].extensionName);
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

  printf("VALIDATION LAYER SUPPORT:\n");
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
    printf("%s\n",app->instance_layer_properties[app->instance_layer_count].layerName);
    app->instance_layer_count++;
  }

  free(vk_props);
  vk_props = NULL;

  return res;
}

/* Create connection between app and the vulkan api */
VkResult create_instance(struct vkcomp *app, char *app_name, char *engine_name) {
  VkResult res = VK_INCOMPLETE;

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

/*
 * Almost every operation in Vulkan, anything from drawing to
 * uploading textures, requires commands to be submitted
 * to a queue. This will create and find
 * which queue families are supported
 * by the device and which one of these supports the
 * commands that we want to use
 */
VkBool32 find_queue_families(struct vkcomp *app) {
  VkBool32 ret = false;
  VkBool32 present_support = false;

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, NULL);
  app->queue_families = calloc(sizeof(VkQueueFamilyProperties), app->queue_family_count * sizeof(VkQueueFamilyProperties));
  assert(app->queue_families != NULL);

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, app->queue_families);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT        ||
        app->queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ||
        app->queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT      ||
        app->queue_families[i].queueFlags & VK_QUEUE_FLAG_BITS_MAX_ENUM)
        app->indices.graphics_family = i;

    /* To look for a queue family with the capabilities to present our window system */
    vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support);

    if (app->indices.graphics_family != -1 && present_support) {
      app->indices.present_family = i;
      ret = true;
      break;
    }
  }

  return ret;
}

VkBool32 is_device_suitable(struct vkcomp *app, VkPhysicalDevice device) {

  /* Query device properties */
  vkGetPhysicalDeviceProperties(device, &app->device_properties);
  vkGetPhysicalDeviceFeatures(device, &app->device_features);

  return ((app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) &&
          app->device_features.geometryShader);
}

/* Get user physical device */
VkResult enumerate_devices(struct vkcomp *app) {
  VkResult res = VK_INCOMPLETE;
  res = vkEnumeratePhysicalDevices(app->instance, &app->device_count, NULL);
  if (res) return res;

  if (app->device_count == 0) {
    perror("[x] failed to find GPUs with Vulkan support!");
    return res;
  }

  app->devices = calloc(sizeof(VkPhysicalDevice), app->device_count * sizeof(VkPhysicalDevice));
  assert(app->devices != NULL);

  res = vkEnumeratePhysicalDevices(app->instance, &app->device_count, app->devices);
  if (res) return res;

  /* get physical device */
  for (uint32_t i = 0; i < app->device_count; i++) {
    if (is_device_suitable(app, app->devices[i])) {
      app->physical_device = app->devices[i];
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    perror("[x] failed to find a suitable GPU!");
    return res;
  }

  /* Query device properties */
  vkGetPhysicalDeviceProperties(app->physical_device, &app->device_properties);
  printf("\nPHYSICAL_DEVICE FOUND: %s\n", app->device_properties.deviceName);

  return res;
}

/* After selecting a physical device to use.
  Set up a logical device to interface with it */
VkResult init_logical_device(struct vkcomp *app) {
  VkQueue present_queue;
  VkResult res = VK_INCOMPLETE;
  float queue_priorities[1] = {1.0};

  app->queue_create_infos = calloc(sizeof(VkDeviceQueueCreateInfo),
      app->queue_family_count * sizeof(VkDeviceQueueCreateInfo));
  assert(app->queue_create_infos != NULL);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    app->queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    app->queue_create_infos[i].pNext = NULL;
    app->queue_create_infos[i].flags = 0;
    app->queue_create_infos[i].queueFamilyIndex = i;
    app->queue_create_infos[i].queueCount = app->queue_family_count;
    app->queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
  create_info.pQueueCreateInfos = app->queue_create_infos;
  create_info.queueCreateInfoCount = app->queue_create_infos[0].queueCount;
  create_info.pEnabledFeatures = &app->device_features;
  create_info.enabledExtensionCount = 0;
  create_info.ppEnabledExtensionNames = NULL;
  create_info.ppEnabledLayerNames = NULL;
  create_info.enabledLayerCount = 0;

  /* Create logic device */
  res = vkCreateDevice(app->physical_device, &create_info, NULL, &app->device);
  if (res) return res;

  /*
   * Queues are automatically created with
   * the logical device, but you need a queue
   * handle to interface with them
   */
  vkGetDeviceQueue(app->device, app->indices.graphics_family, 0, &app->graphics_queue);
  vkGetDeviceQueue(app->device, app->indices.present_family, 0, &present_queue);

  return res;
}

void freeup_vk(struct vkcomp *app) {
  vkDeviceWaitIdle(app->device);
  vkDestroyDevice(app->device, NULL);
  free(app->instance_layer_properties);
  free(app->devices);
  free(app->queue_families);
  free(app->queue_create_infos);
  vkDestroySurfaceKHR(app->instance, app->surface, NULL);
  vkDestroyInstance(app->instance, NULL);
  initialize_vulkan_values(app);
}
