/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Vincent Davis Jr.
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
VkBool32 is_device_suitable(
  VkPhysicalDevice device,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
) {

  vkGetPhysicalDeviceProperties(device, device_props); /* Query device properties */
  vkGetPhysicalDeviceFeatures(device, device_feats); /* Query device features */

  return (device_props->deviceType == vkpdtype    &&
          device_feats->depthClamp                &&
          device_feats->depthBiasClamp            &&
          device_feats->logicOp                   &&
          device_feats->robustBufferAccess);
}

VkResult get_extension_properties(
  vkcomp *app,
  VkPhysicalDevice device,
  VkExtensionProperties **eprops
) {
  VkResult res = VK_INCOMPLETE;
  VkExtensionProperties *extensions = NULL;
  uint32_t extension_count = 0;

  do {
    res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
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

    res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
                  vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);
  } while (res == VK_INCOMPLETE);

  /* set available instance extensions */
  *eprops = (VkExtensionProperties *) calloc(extension_count * sizeof(VkExtensionProperties), sizeof(VkExtensionProperties));
  if (!(*eprops)) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] calloc: %s", strerror(errno));
    goto finish_extensions;
  }

  *eprops = memcpy(*eprops, extensions, extension_count * sizeof(extensions[0]));
  if (!(*eprops)) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] memcpy of VkExtensionProperties *extensions to app->eprops failed");
    goto finish_extensions;
  }

finish_extensions:
  FREE(extensions);
  return res;
}

VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 *present_support = NULL;
  VkQueueFamilyProperties *queue_families = NULL;
  uint32_t qfc = 0; /* queue family count */
  if (!app->physical_device) {
    wlu_log_me(WLU_DANGER, "[x] A physical device must be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_physical_device()");
    goto finish_queue_family;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, NULL);

  queue_families = (VkQueueFamilyProperties *) calloc(
    qfc * sizeof(VkQueueFamilyProperties), sizeof(VkQueueFamilyProperties));
  if (!queue_families) {
    wlu_log_me(WLU_DANGER, "[x] calloc: %s", strerror(errno));
    goto finish_queue_family;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, queue_families);

  present_support = calloc(qfc * sizeof(VkBool32), sizeof(VkBool32));
  if (!present_support) {
    wlu_log_me(WLU_DANGER, "[x] calloc: %s", strerror(errno));
    goto finish_queue_family;
  }

  if (app->surface)
    for (uint32_t i = 0; i < qfc; i++)
      vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support[i]);

  for (uint32_t i = 0; i < qfc; i++) {
    if (queue_families[i].queueFlags & vkqfbits) {
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
    for (uint32_t i = 0; i < qfc; i++) {
      if (present_support[i]) {
        app->indices.present_family = i; ret = VK_FALSE;
        break;
      }
    }
  }

finish_queue_family:
  FREE(queue_families);
  FREE(present_support);
  return ret;
}

/* query device capabilities */
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] A VkSurfaceKHR must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR()");
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
