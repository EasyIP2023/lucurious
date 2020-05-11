/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2020 Vincent Davis Jr.
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

#define LUCUR_VKCOMP_API
#include <lucom.h>

/**
* This function is mainly where we query a given physical device
* properties/features and assign data to addresses
*/
VkBool32 is_device_suitable(
  VkPhysicalDevice device,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
) {

  vkGetPhysicalDeviceProperties(device, device_props); /* Query device properties */
  vkGetPhysicalDeviceFeatures(device, device_feats); /* Query device features */

  return (device_props->deviceType == vkpdtype && device_feats->depthClamp &&
          device_feats->depthBiasClamp         && device_feats->logicOp    &&
          device_feats->robustBufferAccess      && device_feats->samplerAnisotropy);
}

/* Maybe add error messaging here later, hmm... */
VkResult get_extension_properties(
  VkPhysicalDevice device,
  uint32_t *count,
  VkExtensionProperties **eprops
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  res = (!device) ? vkEnumerateInstanceExtensionProperties(NULL, count, NULL) : vkEnumerateDeviceExtensionProperties(device, NULL, count, NULL);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, (!device) ? "vkEnumerateInstanceExtensionProperties" : "vkEnumerateDeviceExtensionProperties"); return res; }

  if (*count == 0) return VK_RESULT_MAX_ENUM;

  /* Allocate space for extensions then set the available instance extensions */
  *eprops = wlu_alloc(WLU_SMALL_BLOCK_PRIV, *count * sizeof(VkExtensionProperties));
  if (!(*eprops)) { PERR(WLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }

  res = (!device) ? vkEnumerateInstanceExtensionProperties(NULL, count, *eprops) : vkEnumerateDeviceExtensionProperties(device, NULL, count, *eprops);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, (!device) ? "vkEnumerateInstanceExtensionProperties" : "vkEnumerateDeviceExtensionProperties"); return res; }

  return res;
}
