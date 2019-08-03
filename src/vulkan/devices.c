#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
#include <vlucur/devices.h>

/*
 * Almost every operation in Vulkan, anything from drawing to
 * uploading textures, requires commands to be submitted
 * to a queue. This will create and find
 * which queue families are supported
 * by the device and which one of these supports the
 * commands that we want to use
 */
VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 present_support = VK_FALSE;

  if (!app->physical_device) {
    wlu_log_me(WLU_DANGER, "[x] A physical device must be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_enumerate_devices(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return ret;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, NULL);
  app->queue_families = (VkQueueFamilyProperties *) calloc(sizeof(VkQueueFamilyProperties),
      app->queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!app->queue_families) {
    wlu_log_me(WLU_DANGER, "[x] realloc of app->queue_families failed");
    return ret;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, app->queue_families);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & vkqfbits) {
      wlu_log_me(WLU_SUCCESS, "Physical Device has support for provided Queue Family");
      app->indices.graphics_family = i;
    }

    /* Check to see if a device can create images on the surface we may have created */
    if (app->surface)
      vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support);

    if (app->indices.graphics_family != UINT32_MAX && present_support) {
      app->indices.present_family = i;
      ret = VK_FALSE;
      wlu_log_me(WLU_SUCCESS, "Physical Device Surface has presentation support");
      break;
    } else if (app->indices.graphics_family != UINT32_MAX) {
      ret = VK_FALSE;
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

  return (app->device_properties.deviceType == vkpdtype       &&
          app->device_features.depthClamp                     &&
          app->device_features.depthBiasClamp                 &&
          app->device_features.logicOp);
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
      wlu_log_me(WLU_DANGER, "[x] realloc of extensions failed");
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
      wlu_log_me(WLU_DANGER, "[x] calloc of app->ep_instance_props failed");
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      app->ep_instance_props[i] = extensions[i];
      app->ep_instance_count = i;
    }
  }

  /* set available device extensions */
  if (device) {
    app->ep_device_props = (VkExtensionProperties *) \
      realloc(app->ep_device_props, extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_device_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc of app->ep_device_props failed");
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      app->ep_device_props[i] = extensions[i];
      app->ep_device_count = i;
    }

    /* check for swap chain support */
    for (uint32_t i = 0; i < app->ep_device_count; i++) {
      if (!strcmp(app->ep_device_props[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
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
