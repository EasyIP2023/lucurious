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
* This is standard for all debug report callbacks.
* The return value must be a Boolean to indicated to the validation layers
* whether the Vulkan API call that triggered the report should be exited or not.
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report_callbackFN(
  VkDebugReportFlagsEXT flags UNUSED,
  VkDebugReportObjectTypeEXT objectType UNUSED,
  uint64_t object UNUSED,
  size_t  location UNUSED,
  int32_t messageCode UNUSED,
  const char *pLayerPrefix UNUSED,
  const char *pMessage,
  void *pUserData UNUSED
) {

  dlu_log_me(DLU_DANGER, "%s", pMessage);

  return VK_FALSE;
}

VkResult dlu_set_debug_message(vkcomp *app, VkDebugReportFlagBitsEXT flags) {
  VkResult res = VK_RESULT_MAX_ENUM;
  PFN_vkCreateDebugReportCallbackEXT dbg_create_report_callback = VK_NULL_HANDLE;

  if (!app->instance) { PERR(DLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  /**
  * Dynamically Dynamically retrieving a VkInstance related function VkCreateDebugReportCallbackEXT
  * to expose the VkDebugReportCallbackEXT handle which stores a reference to a debug report callback object
  */
  DLU_DR_INSTANCE_PROC_ADDR(dbg_create_report_callback, app->instance, CreateDebugReportCallbackEXT);
  if (!dbg_create_report_callback) return VK_ERROR_INITIALIZATION_FAILED;

  /**
  * Dynamically Dynamically retrieving a VkInstance related function VkDestroyDebugReportCallbackEXT
  * to destroy the reference to a debug report callback object
  */
  DLU_DR_INSTANCE_PROC_ADDR(app->dbg_destroy_report_callback, app->instance, DestroyDebugReportCallbackEXT);
  if (!app->dbg_destroy_report_callback) return VK_ERROR_INITIALIZATION_FAILED;

  VkDebugReportCallbackCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.pfnCallback = debug_report_callbackFN;
  create_info.pUserData = NULL;

  /**
  * Create the debug report callback object this allows for the detected validation errors and warnings to be exposed by
  * debug_report_callbackFN. Allow me to edit the object of a given validation layer error.
  */
  res = dbg_create_report_callback(app->instance, &create_info, NULL, &app->debug_report_callback);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkCreateDebugReportCallbackEXT");

  return res;
}

/**
* Set vulkan validation layers properties.
* To get more validation layers install vulkan sdk
*/
VkResult dlu_set_global_layers(VkLayerProperties **vk_props, uint32_t *size) {
  VkResult res = VK_RESULT_MAX_ENUM;

  /* Find the amount of validation layer */
  res = vkEnumerateInstanceLayerProperties(size, NULL);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEnumerateInstanceLayerProperties"); return VK_RESULT_MAX_ENUM; }

  /* layer count will only be zero if vulkan sdk not installed */
  if (*size == 0) {
    dlu_log_me(DLU_WARNING, "[x] failed to find any Validation Layers!!");
    return VK_RESULT_MAX_ENUM;
  }

  /* allocate space */
  *vk_props = dlu_alloc(DLU_SMALL_BLOCK_PRIV, *size * sizeof(VkLayerProperties));
  if (!(*vk_props)) { PERR(DLU_ALLOC_FAILED, 0, NULL); return VK_RESULT_MAX_ENUM; }

  /* set validation layers values */
  res = vkEnumerateInstanceLayerProperties(size, *vk_props);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEnumerateInstanceLayerProperties"); }

  return res;
}
