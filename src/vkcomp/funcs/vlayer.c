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

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report_callbackFN(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objectType,
  uint64_t object,
  size_t  location,
  int32_t messageCode,
  const char *pLayerPrefix,
  const char *pMessage,
  void *pUserData
) {
  ALL_UNUSED(flags, objectType, object, messageCode, location, pLayerPrefix, pUserData);
  wlu_log_me(WLU_DANGER, "%s", pMessage);
  return VK_FALSE;
}

VkResult wlu_set_debug_message(vkcomp *app) {
  VkResult res = VK_RESULT_MAX_ENUM;
  PFN_vkCreateDebugReportCallbackEXT dbg_create_report_callback = VK_NULL_HANDLE;

  if (!app->instance) { PERR(WLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  WLU_DR_INSTANCE_PROC_ADDR(dbg_create_report_callback, app->instance, CreateDebugReportCallbackEXT);
  if (!dbg_create_report_callback) return VK_ERROR_INITIALIZATION_FAILED;

  WLU_DR_INSTANCE_PROC_ADDR(app->dbg_destroy_report_callback, app->instance, DestroyDebugReportCallbackEXT);
  if (!app->dbg_destroy_report_callback) return VK_ERROR_INITIALIZATION_FAILED;

  VkDebugReportCallbackCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
  create_info.pNext = NULL;
  create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
  create_info.pfnCallback = debug_report_callbackFN;
  create_info.pUserData = NULL;

  res = dbg_create_report_callback(app->instance, &create_info, NULL, &app->debug_report_callback);
  if (res) { PERR(WLU_VK_CREATE_ERR, res, "CreateDebugReportCallbackEXT"); }

  return res;
}

/**
* Set vulkan validation layers properties.
* To get more validation layers install vulkan sdk
*/
VkResult wlu_set_global_layers(VkLayerProperties **vk_props, uint32_t *size) {
  VkResult res = VK_INCOMPLETE;

  /* Find the amount of validation layer */
  res = vkEnumerateInstanceLayerProperties(size, NULL);
  if (res) { PERR(WLU_VK_ENUM_ERR, res, "InstanceLayerProperties"); goto finish_vk_props; }

  /* layer count will only be zero if vulkan sdk not installed */
  if (*size == 0) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_WARNING, "[x] failed to find any Validation Layers!!");
    goto finish_vk_props;
  }

  /* allocate space */
  *vk_props = wlu_alloc(WLU_SMALL_BLOCK_PRIV, *size * sizeof(VkLayerProperties));
  if (!(*vk_props)) { PERR(WLU_ALLOC_FAILED, res = VK_RESULT_MAX_ENUM, NULL); goto finish_vk_props; }

  /* set validation layers values */
  res = vkEnumerateInstanceLayerProperties(size, *vk_props);
  if (res) { PERR(WLU_VK_ENUM_ERR, res, "InstanceLayerProperties"); }

finish_vk_props:
  return res;
}
