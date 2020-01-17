/**
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

/* Maybe add error messaging here later, hmm... */
VkResult get_extension_properties(
  vkcomp *app,
  VkPhysicalDevice device,
  VkExtensionProperties **eprops
) {
  VkResult res = VK_INCOMPLETE;
  VkExtensionProperties *extensions = NULL;
  uint32_t extension_count = 0;

  res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
                vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
  if (res) return res;

  /* Rare but may happen for instances. If so continue on with the app */
  if (extension_count == 0) return res;

  extensions = (VkExtensionProperties *) alloca(extension_count * sizeof(VkExtensionProperties));

  res = (app) ? vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions) :
                vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);
  if (res) return res;

  /* set available instance extensions */
  *eprops = wlu_alloc(WLU_SMALL_BLOCK, extension_count * sizeof(VkExtensionProperties));
  if (!(*eprops)) return VK_RESULT_MAX_ENUM;

  *eprops = memmove(*eprops, extensions, extension_count * sizeof(extensions[0]));
  if (!(*eprops)) {
    wlu_log_me(WLU_DANGER, "[x] memcpy of VkExtensionProperties *extensions to app->eprops failed");
    return VK_RESULT_MAX_ENUM;
  }

  return res;
}

VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 *present_support = NULL;
  VkQueueFamilyProperties *queue_families = NULL;
  uint32_t qfc = 0; /* queue family count */

  if (!app->physical_device) { PERR(WLU_VKCOMP_PHYSICAL_DEV, 0, NULL); return ret; }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, NULL);

  queue_families = (VkQueueFamilyProperties *) alloca(qfc * sizeof(VkQueueFamilyProperties));

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, queue_families);

  present_support = alloca(qfc * sizeof(VkBool32));

  if (app->surface)
    for (uint32_t i = 0; i < qfc; i++) /* Check for present queue family */
      vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support[i]);

  for (uint32_t i = 0; i < qfc; i++) {
    if (queue_families[i].queueFlags & vkqfbits) {
      if (app->indices.graphics_family == UINT32_MAX) {
        /* Retrieve Graphics Family Queue index */
        app->indices.graphics_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device has support for provided Queue Family");
      }

      /* Check to see if a device can create images on the surface we may have created */
      if (app->surface && present_support[i]) {
        /* Retrieve Present Family Queue index */
        app->indices.present_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device Surface has presentation support");
        break;
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

  return ret;
}

/* query device capabilities */
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app) {
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult err;

  if (!app->surface) {
    PERR(WLU_VKCOMP_SURFACE, 0, NULL);
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->physical_device, app->surface, &capabilities);
  if (err) {
    PERR(WLU_VK_GET_ERR, err, "PhysicalDeviceSurfaceCapabilitiesKHR");
    capabilities.minImageCount = UINT32_MAX;
    return capabilities;
  }

  return capabilities;
}
