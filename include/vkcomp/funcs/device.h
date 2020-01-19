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

#ifndef WLU_VKCOMP_DEVICE_FUNCS_H
#define WLU_VKCOMP_DEVICE_FUNCS_H

/**
* Almost every operation in Vulkan, from submitting command buffers
* to presenting images to a surface, requires commands to be submitted
* to a hardware queue. This will create multiple queue families
* that are supported by the VkQueueFlagBits set and assign the
* available graphics and present queues
*/
VkBool32 wlu_set_queue_family(vkcomp *app, VkQueueFlagBits vkqfbits);

/**
* Needed to create the swap chain. This function queries your physical device's
* capabilities. Mainly used to get minImageCount and the extent/resolution
* that a particular physical device
*/
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app);

#ifdef INAPI_CALLS
VkBool32 is_device_suitable(
  VkPhysicalDevice device,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
);

VkResult get_extension_properties(
  vkcomp *app,
  VkPhysicalDevice device,
  VkExtensionProperties **eprops
);
#endif

#endif
