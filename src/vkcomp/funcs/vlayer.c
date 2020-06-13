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
* The return value must be a Boolean to indicated to the validation layers
* whether the Vulkan API call that triggered the report should be exited or not.
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_callbackFN(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT UNUSED messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void UNUSED *pUserData
) {

  dlu_log_type type = DLU_NONE;

  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: type = DLU_INFO; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: type = DLU_WARNING; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: type = DLU_DANGER; break;
    default: break;
  }

//  dlu_log_me(type, "Message ID: %d", pCallbackData->messageIdNumber);
//  dlu_log_me(type, "Message ID Name: %s", pCallbackData->pMessageIdName);

  dlu_log_me(type, "%s", pCallbackData->pMessage);

  return VK_FALSE;
}

VkResult dlu_set_debug_message(
  vkcomp *app,
  int flags,
  VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType
) {

  VkResult res = VK_RESULT_MAX_ENUM;
  PFN_vkCreateDebugUtilsMessengerEXT dbg_create_utils_msg = VK_NULL_HANDLE;

  if (!app->instance) { PERR(DLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  /**
  * Dynamically retrieving a VkInstance related function vkCreateDebugUtilsMessengerEXT
  * to expose the VkDebugReportCallbackEXT handle which stores a reference to a debug utils message object
  * Basically call extension functions through a function pointer
  */
  DLU_DR_INSTANCE_PROC_ADDR(dbg_create_utils_msg, app->instance, CreateDebugUtilsMessengerEXT);
  if (!dbg_create_utils_msg) return VK_ERROR_INITIALIZATION_FAILED;

  /**
  * Dynamically retrieving a VkInstance related function vkDestroyDebugUtilsMessengerEXT
  * to destroy the reference to a debug utils message object.
  * Basically call extension functions through a function pointer
  */
  DLU_DR_INSTANCE_PROC_ADDR(app->dbg_destroy_utils_msg, app->instance, DestroyDebugUtilsMessengerEXT);
  if (!app->dbg_destroy_utils_msg) return VK_ERROR_INITIALIZATION_FAILED;

  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.messageSeverity = messageSeverity;
  create_info.messageType = messageType;
  create_info.pfnUserCallback = debug_utils_callbackFN;
  create_info.pUserData = NULL;

  /**
  * Create the debug utils message object this allows for the detected validation errors
  * and warnings to be exposed by debug_report_callbackFN.
  */
  res = dbg_create_utils_msg(app->instance, &create_info, NULL, &app->debug_utils_msg);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkCreateDebugUtilsMessengerEXT");

  return res;
}

VkResult dlu_set_device_debug_ext(vkcomp *app) {

  if (!app->device) { PERR(DLU_VKCOMP_DEVICE, 0, NULL); return VK_RESULT_MAX_ENUM; }

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_queue_begin, app->device, QueueBeginDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_begin) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_queue_end, app->device, QueueEndDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_end) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_queue_insert, app->device, QueueInsertDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_insert) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_cmd_begin, app->device, CmdBeginDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_begin) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_cmd_end, app->device, CmdEndDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_end) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->dbg_utils_cmd_insert, app->device, CmdInsertDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_insert) return VK_ERROR_INITIALIZATION_FAILED;

  return VK_SUCCESS;
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
