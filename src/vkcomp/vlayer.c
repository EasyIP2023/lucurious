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

static const char *obj_to_str(const VkObjectType type) {
  switch(type) {
    case VK_OBJECT_TYPE_INSTANCE: return "VkInstance";
    case VK_OBJECT_TYPE_PHYSICAL_DEVICE: return "VkPhysicalDevice";
    case VK_OBJECT_TYPE_DEVICE: return "VkDevice";
    case VK_OBJECT_TYPE_QUEUE: return "VkQueue";
    case VK_OBJECT_TYPE_SEMAPHORE: return "VkSemaphore";
    case VK_OBJECT_TYPE_COMMAND_BUFFER: return "VkCommandBuffer";
    case VK_OBJECT_TYPE_FENCE: return "VkFence";
    case VK_OBJECT_TYPE_DEVICE_MEMORY: return "VkDeviceMemory";
    case VK_OBJECT_TYPE_BUFFER: return "VkBuffer";
    case VK_OBJECT_TYPE_IMAGE: return "VkImage";
    case VK_OBJECT_TYPE_EVENT: return "VkEvent";
    case VK_OBJECT_TYPE_QUERY_POOL: return "VkQueryPool";
    case VK_OBJECT_TYPE_BUFFER_VIEW: return "VkBufferView";
    case VK_OBJECT_TYPE_IMAGE_VIEW: return "VkImageView";
    case VK_OBJECT_TYPE_SHADER_MODULE: return "VkShaderModule";
    case VK_OBJECT_TYPE_PIPELINE_CACHE: return "VkPipelineCache";
    case VK_OBJECT_TYPE_PIPELINE_LAYOUT: return "VkPipelineLayout";
    case VK_OBJECT_TYPE_RENDER_PASS: return "VkRenderPass";
    case VK_OBJECT_TYPE_PIPELINE: return "VkPipeline";
    case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT: return "VkDescriptorSetLayout";
    case VK_OBJECT_TYPE_SAMPLER: return "VkSampler";
    case VK_OBJECT_TYPE_DESCRIPTOR_POOL: return "VkDescriptorPool";
    case VK_OBJECT_TYPE_DESCRIPTOR_SET: return "VkDescriptorSet";
    case VK_OBJECT_TYPE_FRAMEBUFFER: return "VkFramebuffer";
    case VK_OBJECT_TYPE_COMMAND_POOL: return "VkCommandPool";
    case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION: return "VkSamplerYcbcrConversion";
    case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE: return "VkDescriptorUpdateTemplate";
    case VK_OBJECT_TYPE_SURFACE_KHR: return "VkSurfaceKHR";
    case VK_OBJECT_TYPE_SWAPCHAIN_KHR: return "VkSwapchainKHR";
    case VK_OBJECT_TYPE_DISPLAY_KHR: return "VkDisplayKHR";
    case VK_OBJECT_TYPE_DISPLAY_MODE_KHR: return "VkDisplayModeKHR";
    case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT: return "VkDebugReportCallbackEXT";
    case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV: return "VkIndirectCommandsLayoutNV";
    case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT: return "VkDebugUtilsMessengerEXT";
    case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT: return "VkValidationCacheEXT";
    case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR: return "VkAccelerationStructureKHR";
    case VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL: return "VkPerformanceConfigurationINTEL";
    default: return "Unknown/Undefined Handle";
  }
}

/**
* The return value must be a Boolean to indicated to the validation layers
* whether the Vulkan API call that triggered the report should be exited or not.
* Due to this function only being used when debugging secure buffer boundary
* base C functions are not needed.
* https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_callbackFN(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void UNUSED *pUserData
) {

  dlu_log_type type = DLU_NONE;

  char prefix[64];
  uint32_t str_sze = strlen(pCallbackData->pMessage) + 1000;
  char *message = (char *) alloca(str_sze);

  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      strcpy(prefix, "\n\nValidation Verbose "); type = DLU_INFO;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      strcpy(prefix, "\n\nValidation Info "); type = DLU_INFO;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      strcpy(prefix, "\n\nValidation Warning "); type = DLU_WARNING;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
       strcpy(prefix, "\n\nValidation Error "); type = DLU_DANGER;
       break;
    default: break;
  }

  switch (messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      strcat(prefix, "General: ");
      sprintf(message, "%s MessageID = 0x%x" PRIu32 ", MessageID Name = [ %s ]\n\t  %s\n\n",
              prefix, pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
      break; /* Leave bellow for now */
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      sprintf(message, "\n\n%s\n\n", pCallbackData->pMessage);
      break;
    default: break;
  }

  char tmp_msg[1000];
  if (pCallbackData->objectCount > 0) {
    snprintf(tmp_msg, sizeof(tmp_msg), "\t  Objects - %d\n", pCallbackData->objectCount);
    strncat(message, tmp_msg, str_sze);

    for (unsigned object = 0; object < pCallbackData->objectCount; object++) {
      snprintf(tmp_msg, sizeof(tmp_msg), "\t\tObject[%d] : Type = %s, Handle = %p, Name = \"%s\"\n", object, obj_to_str(pCallbackData->pObjects[object].objectType),
              (void *) (pCallbackData->pObjects[object].objectHandle), pCallbackData->pObjects[object].pObjectName);
      strncat(message, tmp_msg, str_sze);
    }
    memset(tmp_msg, 0, sizeof(tmp_msg));
  }

  if (pCallbackData->cmdBufLabelCount > 0) {
    snprintf(tmp_msg, sizeof(tmp_msg), "VkCommandBuffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
    strncat(message, tmp_msg, str_sze);
    for (uint32_t label = 0; label < pCallbackData->cmdBufLabelCount; label++) {
      snprintf(tmp_msg, sizeof(tmp_msg), "\tLabel[%d] : %s { %f, %f, %f, %f }\n",
               label, pCallbackData->pCmdBufLabels[label].pLabelName,
               pCallbackData->pCmdBufLabels[label].color[0], pCallbackData->pCmdBufLabels[label].color[1],
               pCallbackData->pCmdBufLabels[label].color[2], pCallbackData->pCmdBufLabels[label].color[3]);
      strncat(message, tmp_msg, str_sze);
    }
    memset(tmp_msg, 0, sizeof(tmp_msg));
  }

  if (pCallbackData->queueLabelCount > 0) {
    snprintf(tmp_msg, sizeof(tmp_msg), "VkQueue Labels - %d\n", pCallbackData->queueLabelCount);
    strncat(message, tmp_msg, str_sze);
    for (uint32_t label = 0; label < pCallbackData->queueLabelCount; label++) {
      snprintf(tmp_msg, sizeof(tmp_msg), "\tLabel[%d] : %s { %f, %f, %f, %f }\n",
               label, pCallbackData->pQueueLabels[label].pLabelName,
               pCallbackData->pQueueLabels[label].color[0], pCallbackData->pQueueLabels[label].color[1],
               pCallbackData->pQueueLabels[label].color[2], pCallbackData->pQueueLabels[label].color[3]);
      strncat(message, tmp_msg, str_sze);
    }
    memset(tmp_msg, 0, sizeof(tmp_msg));
  }

  dlu_log_me(type, "%s", message);

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
  DLU_DR_INSTANCE_PROC_ADDR(app->instance, dbg_create_utils_msg, CreateDebugUtilsMessengerEXT);
  if (!dbg_create_utils_msg) return VK_ERROR_INITIALIZATION_FAILED;

  /**
  * Dynamically retrieving a VkInstance related function vkDestroyDebugUtilsMessengerEXT
  * to destroy the reference to a debug utils message object.
  * Basically call extension functions through a function pointer
  */
  DLU_DR_INSTANCE_PROC_ADDR(app->instance, app->dbg_destroy_utils_msg, DestroyDebugUtilsMessengerEXT);
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

VkResult dlu_set_device_debug_ext(vkcomp *app, uint32_t cur_ld) {

  if (!app->ld_data[cur_ld].device) { PERR(DLU_VKCOMP_DEVICE, 0, NULL); return VK_RESULT_MAX_ENUM; }

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_queue_begin, QueueBeginDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_begin) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_queue_end, QueueEndDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_end) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_queue_insert, QueueInsertDebugUtilsLabelEXT);
  if (!app->dbg_utils_queue_insert) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_cmd_begin, CmdBeginDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_begin) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_cmd_end, CmdEndDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_end) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_cmd_insert, CmdInsertDebugUtilsLabelEXT);
  if (!app->dbg_utils_cmd_insert) return VK_ERROR_INITIALIZATION_FAILED;

  DLU_DR_DEVICE_PROC_ADDR(app->ld_data[cur_ld].device, app->dbg_utils_set_object_name, SetDebugUtilsObjectNameEXT);
  if (!app->dbg_utils_set_object_name) return VK_ERROR_INITIALIZATION_FAILED;

  return VK_SUCCESS;
}

/**
* Set vulkan Vulkan layers properties.
* To get more validation layers install vulkan sdk
*/
VkResult dlu_set_vulkan_layer_props(VkLayerProperties **vk_props, uint32_t *size) {
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
