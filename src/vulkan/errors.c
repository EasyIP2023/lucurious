#include <vlucur/errors.h>

#include <stdio.h>
#include <stdlib.h>

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    // write important message
    fprintf(stderr, "\t%d\n", pCallbackData->objectCount);
  }

  /* Something has happened that violates the
    specification or indicates a possible mistake */
  if (messageType >= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
    fprintf(stdout, "\t%s\n", pCallbackData->pMessage); // void * but set to string for now
  }

  if (pUserData != NULL)
    return VK_FALSE;

  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) \
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != NULL) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger, const
    VkAllocationCallbacks* pAllocator) {

  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) \
  vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func != NULL) {
    func(instance, debugMessenger, pAllocator);
  }
}
