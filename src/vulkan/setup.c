#include <vlucur/vkall.h>
#include <vlucur/devices.h>

struct vkcomp init_vk() {
  struct vkcomp app;
  app.instance = 0;
  app.surface = VK_NULL_HANDLE;
  app.instance_layer_properties = NULL;
  app.instance_layer_count = 0;
  app.physical_device = VK_NULL_HANDLE;
  app.devices = VK_NULL_HANDLE;
  app.device_count = 0;
  app.queue_families = NULL;
  app.queue_family_count = 0;
  app.indices.graphics_family = -1;
  app.indices.present_family = -1;
  app.device = VK_FALSE;
  app.graphics_queue = VK_FALSE;
  app.queue_create_infos = NULL;
  return app;
}

/*
 * Gets all you're validation layers extensions
 * that you can later using pointer arithmetic to
 * cycle through
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
    if (!vk_props) return res;

    res = vkEnumerateInstanceLayerProperties(&layer_count, vk_props);
  } while (res == VK_INCOMPLETE);

  if (app->instance_layer_properties == NULL)
    app->instance_layer_properties = (VkLayerProperties *) \
      calloc(sizeof(VkLayerProperties), layer_count * sizeof(VkLayerProperties));

  fprintf(stdout, "\nVALIDATION LAYER SUPPORT:\n");
  /* Gather the extension list for each instance layer. */
  for (uint32_t i = 0; i < layer_count; i++) {
    res = get_extension_properties(NULL, &vk_props[i], NULL);
    if (res) return res;

    /* must to do a deep copy of the structs information */
    memcpy(app->instance_layer_properties[app->instance_layer_count].layerName,
           vk_props[i].layerName, strlen(vk_props[i].layerName) + 1);
    memcpy(app->instance_layer_properties[app->instance_layer_count].description,
           vk_props[i].description, strlen(vk_props[i].description) + 1);
    app->instance_layer_properties[app->instance_layer_count].specVersion = vk_props[i].specVersion;
    app->instance_layer_properties[app->instance_layer_count].implementationVersion = vk_props[i].implementationVersion;
    fprintf(stdout, "%s\n",app->instance_layer_properties[app->instance_layer_count].layerName);
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
  create_info.ppEnabledExtensionNames = device_extensions; // assuming device has swap chain support
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

  res = get_extension_properties(app, NULL, NULL);
  if (res) return res;

  return res;
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

  app->devices = (VkPhysicalDevice *) calloc(sizeof(VkPhysicalDevice),
      app->device_count * sizeof(VkPhysicalDevice));
  if (!app->devices) return res;

  res = vkEnumeratePhysicalDevices(app->instance, &app->device_count, app->devices);
  if (res) return res;

  /*
  * get a physical device that is suitable
  * to do the graphics related task that we need
  */
  for (uint32_t i = 0; i < app->device_count; i++) {
    if (is_device_suitable(app, app->devices[i]) &&
        find_queue_families(app, app->devices[i]) &&
        /* Checking for swap chain support */
        get_extension_properties(app, NULL, app->devices[i])) {
      app->physical_device = app->devices[i];
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    perror("[x] failed to find a suitable GPU!");
    return res;
  }

  /* Query device properties */
  vkGetPhysicalDeviceMemoryProperties(app->physical_device, &app->memory_properties);
  vkGetPhysicalDeviceProperties(app->physical_device, &app->device_properties);
  fprintf(stdout, "\nPHYSICAL_DEVICE FOUND: %s\nMEMORY HEAP COUNT: %d\n",
    app->device_properties.deviceName, app->memory_properties.memoryHeapCount);

  return res;
}

/* After selecting a physical device to use.
  Set up a logical device to interface with it */
VkResult init_logical_device(struct vkcomp *app) {
  VkQueue present_queue;
  VkResult res = VK_INCOMPLETE;
  float queue_priorities[1] = {1.0};

  app->queue_create_infos = (VkDeviceQueueCreateInfo *) calloc(sizeof(VkDeviceQueueCreateInfo),
      app->queue_family_count * sizeof(VkDeviceQueueCreateInfo));
  if (!app->queue_create_infos) return res;

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
  create_info.ppEnabledExtensionNames = device_extensions; // assuming device has swap chain support
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
  app->indices.present_family = -1;
  app->device = VK_FALSE;
  app->graphics_queue = VK_FALSE;
  app->queue_create_infos = NULL;
}