#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
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
VkBool32 find_queue_families(vkcomp *app, VkPhysicalDevice device, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_FALSE;
  VkBool32 present_support = VK_FALSE;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, NULL);
  app->queue_families = (VkQueueFamilyProperties *) realloc(app->queue_families,
      app->queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!app->queue_families) {
    wlu_log_me(WLU_DANGER, "[x] realloc of app->queue_families failed: %p - %p",
               &app->queue_families, app->queue_families);
    return ret;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(device, &app->queue_family_count, app->queue_families);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & vkqfbits) {
      wlu_log_me(WLU_SUCCESS, "Physical Device has support for provided Queue Family");
      app->indices.graphics_family = i;
    }

    /* Check to see if a device can create images on the surface we may have created */
    if (app->surface)
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app->surface, &present_support);

    if (app->indices.graphics_family != UINT32_MAX && present_support) {
      app->indices.present_family = i;
      ret = VK_TRUE;
      wlu_log_me(WLU_SUCCESS, "Physical Device Surface has presentation support");
      break;
    } else if (app->indices.graphics_family != UINT32_MAX) {
      ret = VK_TRUE;
      break;
    }
  }

  return ret;
}

VkBool32 is_device_suitable(vkcomp *app, VkPhysicalDevice device, VkPhysicalDeviceType vkpdtype) {
  /* Query device properties */
  vkGetPhysicalDeviceProperties(device, &app->device_properties);
  /* Query device features */
  vkGetPhysicalDeviceFeatures(device, &app->device_features);

  return (app->device_properties.deviceType == vkpdtype && app->device_features.geometryShader);
}

VkResult get_extension_properties(vkcomp *app, VkLayerProperties *prop, VkPhysicalDevice device) {
  VkResult res = VK_INCOMPLETE;
  VkExtensionProperties *extensions = NULL;
  uint32_t extension_count = 0;

  do {
    res = (app && !device)  ?  vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
          (prop)            ?  vkEnumerateInstanceExtensionProperties(prop->layerName, &extension_count, NULL) :
                               vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    if (res) return res;

    /* Rare but may happen for instances. If so continue on with the app */
    if (extension_count == 0) {
      if (app && !device)
        wlu_log_me(WLU_WARNING, "[x] Failed to find instance extensions, extension_count equals 0");
      else if (prop)
        wlu_log_me(WLU_WARNING, "[x] Failed to find validation layers, extension_count equals 0");
      else
        wlu_log_me(WLU_WARNING, "[x] Failed to find device extensions, extension_count equals 0");
      goto finish_extensions;
    }

    extensions = (VkExtensionProperties *) realloc(extensions,
      extension_count * sizeof(VkExtensionProperties));
    if (!extensions) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc of extensions failed: %p - %p",
                 &extensions, extensions);
      goto finish_extensions;
    }

    res = (app && !device)  ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
          (prop)            ? vkEnumerateInstanceExtensionProperties(prop->layerName, &extension_count, extensions) :
                              vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);
  } while (res == VK_INCOMPLETE);

  /* set available instance extensions */
  if (app && !device) {
    app->ep_instance_props = (VkExtensionProperties *) \
      calloc(sizeof(VkExtensionProperties), extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_instance_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] calloc of app->ep_instance_props failed: %p - %p",
                 &app->ep_instance_props, app->ep_instance_props);
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      memcpy(&app->ep_instance_props[i], &extensions[i], sizeof(extensions[i]));
      app->ep_instance_count = i;
    }
  }

  /* set available device extensions */
  if (device) {
    app->ep_device_props = (VkExtensionProperties *) \
      realloc(app->ep_device_props, extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_device_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc of app->ep_device_props failed: %p - %p",
                 &app->ep_device_props, app->ep_device_props);
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      memcpy(&app->ep_device_props[i], &extensions[i], sizeof(extensions[i]));
      app->ep_device_count = i;
    }

    /* check for swap chain support */
    for (uint32_t i = 0; i < app->ep_device_count; i++) {
      if (!strcmp(app->ep_device_props[i].extensionName, device_extensions[0])) {
        res = VK_TRUE;
        wlu_log_me(WLU_SUCCESS, "Physical Device has swap chain support");
        break;
      }
    }
  }

finish_extensions:
  if (extensions) {
    free(extensions);
    extensions = NULL;
  }
  return res;
}
