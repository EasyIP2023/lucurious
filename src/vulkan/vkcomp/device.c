/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
#include <vlucur/device.h>

/* This function is mainly the point where we query a given physical device properties/features */
VkBool32 is_device_suitable(vkcomp *app, VkPhysicalDevice device, VkPhysicalDeviceType vkpdtype) {
  /* Query device properties */
  vkGetPhysicalDeviceProperties(device, &app->device_properties);
  /* Query device features */
  vkGetPhysicalDeviceFeatures(device, &app->device_features);

  return (app->device_properties.deviceType == vkpdtype       &&
          app->device_features.depthClamp                     &&
          app->device_features.depthBiasClamp                 &&
          app->device_features.logicOp                        &&
          app->device_features.robustBufferAccess);
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
    if (extension_count == 0) goto finish_extensions;

    extensions = (VkExtensionProperties *) realloc(extensions,
      extension_count * sizeof(VkExtensionProperties));
    if (!extensions) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc of VkExtensionProperties *extensions failed");
      goto finish_extensions;
    }

    res = (app && !device)  ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
          (prop)            ? vkEnumerateInstanceExtensionProperties(prop->layerName, &extension_count, extensions) :
                              vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);
  } while (res == VK_INCOMPLETE);

  /* set available instance extensions */
  if (app && !device) {
    app->ep_instance_props = (VkExtensionProperties *) calloc(sizeof(VkExtensionProperties),
            extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_instance_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] calloc of VkExtensionProperties *ep_instance_props failed");
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      app->ep_instance_props[i] = extensions[i];
      app->eic = i;
    }
  }

  /* set available device extensions */
  if (device) {
    app->ep_device_props = (VkExtensionProperties *) \
      realloc(app->ep_device_props, extension_count * sizeof(VkExtensionProperties));
    if (!app->ep_device_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc of VkExtensionProperties *ep_device_props failed");
      goto finish_extensions;
    }

    for (uint32_t i = 0; i < extension_count; i++) {
      app->ep_device_props[i] = extensions[i];
      app->edc = i;
    }

    /* check for swap chain support */
    for (uint32_t i = 0; i < app->edc; i++) {
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

VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 *present_support = NULL;

  if (!app->physical_device) {
    wlu_log_me(WLU_DANGER, "[x] A physical device must be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_physical_device(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_queue_family;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, NULL);

  app->queue_families = (VkQueueFamilyProperties *) calloc(sizeof(VkQueueFamilyProperties),
      app->queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!app->queue_families) {
    wlu_log_me(WLU_DANGER, "[x] calloc of VkQueueFamilyProperties *queue_families failed");
    goto finish_queue_family;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &app->queue_family_count, app->queue_families);

  present_support = calloc(sizeof(VkBool32), app->queue_family_count * sizeof(VkBool32));
  if (!present_support) {
    wlu_log_me(WLU_DANGER, "[x] calloc of VkBool32 *present_support failed");
    goto finish_queue_family;
  }

  if (app->surface)
    for (uint32_t i = 0; i < app->queue_family_count; i++)
      vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support[i]);

  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    if (app->queue_families[i].queueFlags & vkqfbits) {
      if (app->indices.graphics_family == UINT32_MAX) {
        app->indices.graphics_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device has support for provided Queue Family");
      }

      /* Check to see if a device can create images on the surface we may have created */
      if (app->surface && present_support[i]) {
        app->indices.present_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device Surface has presentation support");
        goto finish_queue_family;
      }
    }
  }

  if (app->surface && app->indices.present_family == UINT32_MAX) {
    for (uint32_t i = 0; i < app->queue_family_count; i++) {
      if (present_support[i]) {
        app->indices.present_family = i; ret = VK_FALSE;
        break;
      }
    }
  }

finish_queue_family:
  if (present_support) {
    free(present_support);
    present_support = NULL;
  }
  return ret;
}

/* query device capabilities */
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &capabilities);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed, ERROR CODE: %d", err);
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  return capabilities;
}
