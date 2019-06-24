#include <lucom.h>
#include <vlucur/vkall.h>
#include <vlucur/devices.h>

const char *device_extensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char *instance_extensions[] = {
  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  VK_KHR_SURFACE_EXTENSION_NAME,
  VK_KHR_DISPLAY_EXTENSION_NAME
};

/* All of the useful standard validation is
  bundled into a layer included in the SDK */
const char *enabled_validation_layers[] = {
  "VK_LAYER_LUNARG_core_validation", "VK_LAYER_KHRONOS_validation",
  "VK_LAYER_LUNARG_monitor", "VK_LAYER_LUNARG_api_dump",
  "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_object_tracker",
  "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_vktrace",
  "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_GOOGLE_unique_objects",
  "VK_LAYER_LUNARG_assistant_layer", "VK_LAYER_LUNARG_screenshot",
  "VK_LAYER_LUNARG_device_simulation"
};

/*
 * Almost every operation in Vulkan, anything from drawing to
 * uploading textures, requires commands to be submitted
 * to a queue. This will create and find
 * which queue families are supported
 * by the device and which one of these supports the
 * commands that we want to use
 */
VkBool32 find_queue_families(struct vkcomp *app, VkPhysicalDevice device) {
  VkBool32 ret = VK_FALSE;
  VkBool32 present_support = VK_FALSE;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, NULL);
  app->queue_families = (VkQueueFamilyProperties *) realloc(app->queue_families,
      app->queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!app->queue_families) return ret;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, app->queue_families);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT        ||
        app->queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT       ||
        app->queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ||
        app->queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT      ||
        app->queue_families[i].queueFlags & VK_QUEUE_FLAG_BITS_MAX_ENUM)
        app->indices.graphics_family = i;

    /* Check to see if a device can create images on the surface we may have created */
    if (app->surface)
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app->surface, &present_support);

    if (app->indices.graphics_family != -1 && present_support) {
      app->indices.present_family = i;
      ret = VK_TRUE;
      break;
    } else if (app->indices.graphics_family != -1) {
      ret = VK_TRUE;
      break;
    }
  }

  return ret;
}

VkBool32 is_device_suitable(struct vkcomp *app, VkPhysicalDevice device) {
  /* Query device properties */
  vkGetPhysicalDeviceProperties(device, &app->device_properties);
  /* Query device features */
  vkGetPhysicalDeviceFeatures(device, &app->device_features);

  return ((app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ||
          app->device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) &&
          app->device_features.geometryShader);
}

VkResult get_extension_properties(struct vkcomp *app, VkLayerProperties *prop, VkPhysicalDevice device) {
  VkResult res = VK_INCOMPLETE;
  VkExtensionProperties *extensions = NULL;
  uint32_t extension_count = 0;

  do {
    res = (app && !device)  ?  vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
          (prop)            ?  vkEnumerateInstanceExtensionProperties(prop->layerName, &extension_count, NULL) :
                               vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    if (res) return res;

    /* Rare but may happen for instances. If so continue on with the app */
    if (extension_count == 0)
      return VK_SUCCESS;

    extensions = (VkExtensionProperties *) realloc(extensions,
      extension_count * sizeof(VkExtensionProperties));
    if (!extensions) return res;

    res = (app && !device)  ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
          (prop)            ? vkEnumerateInstanceExtensionProperties(prop->layerName, &extension_count, extensions) :
                              vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);
  } while (res == VK_INCOMPLETE);

  /* set available instance extensions */
  if (app && !device) {
    app->ep_instance_props = (VkExtensionProperties *) \
      calloc(sizeof(VkExtensionProperties), extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_instance_props) return res;

    for (uint32_t i = 0; i < extension_count; i++) {
      memcpy(&app->ep_instance_props[i], &extensions[i], sizeof(extensions[i]));
      app->ep_instance_count = i;
    }
  }

  /* set available device extensions */
  if (device) {
    app->ep_device_props = (VkExtensionProperties *) \
      realloc(app->ep_device_props, extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_device_props) return res;

    for (uint32_t i = 0; i < extension_count; i++) {
      memcpy(&app->ep_device_props[i], &extensions[i], sizeof(extensions[i]));
      app->ep_device_count = i;
    }

    /* check for swap chain support */
    for (uint32_t i = 0; i < app->ep_device_count; i++) {
      if (!strcmp(app->ep_device_props[i].extensionName, device_extensions[0])) {
        res = VK_TRUE;
        break;
      }
    }
  }

  free(extensions);
  extensions = NULL;

  return res;
}
