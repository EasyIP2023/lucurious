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

#include <lucom.h>

/**
* Taken from lunarG vulkan API
* https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkResult.html
* I decided to redefine becuase I want to keep utils folder code seperate from vulkan code
*/
enum vkresult_redefined {
  VK_ERROR_OUT_OF_HOST_MEMORY = -1,
  VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
  VK_ERROR_INITIALIZATION_FAILED = -3,
  VK_ERROR_DEVICE_LOST = -4,
  VK_ERROR_MEMORY_MAP_FAILED = -5,
  VK_ERROR_LAYER_NOT_PRESENT = -6,
  VK_ERROR_EXTENSION_NOT_PRESENT = -7,
  VK_ERROR_FEATURE_NOT_PRESENT = -8,
  VK_ERROR_INCOMPATIBLE_DRIVER = -9,
  VK_ERROR_TOO_MANY_OBJECTS = -10,
  VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
  VK_ERROR_FRAGMENTED_POOL = -12,
  VK_ERROR_UNKNOWN = -13,
  VK_ERROR_OUT_OF_POOL_MEMORY = -1000069000,
  VK_ERROR_INVALID_EXTERNAL_HANDLE = -1000072003,
  VK_ERROR_FRAGMENTATION = -1000161000,
  VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS = -1000257000,
  VK_ERROR_SURFACE_LOST_KHR = -1000000000,
  VK_ERROR_NATIVE_WINDOW_IN_USE_KHR = -1000000001,
  VK_SUBOPTIMAL_KHR = 1000001003,
  VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
  VK_ERROR_INCOMPATIBLE_DISPLAY_KHR = -1000003001,
  VK_ERROR_VALIDATION_FAILED_EXT = -1000011001,
  VK_ERROR_INVALID_SHADER_NV = -1000012000,
  VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT = -1000158000,
  VK_ERROR_NOT_PERMITTED_EXT = -1000174001,
  VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT = -1000255000,
  VK_ERROR_OUT_OF_POOL_MEMORY_KHR = VK_ERROR_OUT_OF_POOL_MEMORY,
  VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR = VK_ERROR_INVALID_EXTERNAL_HANDLE,
  VK_ERROR_FRAGMENTATION_EXT = VK_ERROR_FRAGMENTATION,
  VK_ERROR_INVALID_DEVICE_ADDRESS_EXT = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,
  VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS
};

static const char *vkres_msg(int err) {
  switch (err) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "host memory allocation has failed";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "device memory allocation has failed";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "initialization of an object could not be completed for implementation-specific reasons";
    case VK_ERROR_DEVICE_LOST:
      return "the logical or physical device has been lost";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "mapping of a memory object has failed";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "a requested layer is not present or could not be loaded";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "a requested extension is not supported";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "a requested feature is not supported";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "the requested version of Vulkan is not supported by the driver or is otherwise "  \
             "incompatible for implementation-specific reasons";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "too many objects of the type have already been created";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "a requested format is not supported on this device";
    case VK_ERROR_FRAGMENTED_POOL:
      return "a pool allocation has failed due to fragmentation of the pool's memory";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "a surface is no longer available";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "the requested window is already in use by Vulkan or another API in a manner which " \
             "prevents it from being used again";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "a surface has changed in such a way that it is no longer compatible with the swapchain, " \
             "any further presentation requests using the swapchain will fail";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "The display used by a swapchain does not use the same presentable image layout, or is " \
             "incompatible in a way that prevents sharing an image";
    case VK_ERROR_INVALID_SHADER_NV:
      return "one or more shaders failed to compile or link";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "a pool memory allocation has failed";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      return "an external handle is not a valid handle of the specified type";
    case VK_ERROR_FRAGMENTATION:
      return "a descriptor pool creation has failed due to fragmentation";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
      return "a buffer creation or memory allocation failed because the requested address is not available";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
      return "an operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as " \
             "it did not have exlusive full-screen access";
  }

  return "An unknown error has occurred. Either the application has provided invalid input, or an implementation failure has occurred";
}

void _show_err_msg(uint32_t dlu_err, int vkerr, const char *dlu_msg) {
  switch (dlu_err) {
    case DLU_DR_INSTANCE_PROC_ADDR_ERR:
      dlu_log_me(DLU_DANGER, "[x] GetInstanceProcAddr: Unable to find %s function", dlu_msg);
      break;
    case DLU_DR_DEVICE_PROC_ADDR_ERR:
      dlu_log_me(DLU_DANGER, "[x] vkGetDeviceProcAddr: Unable to find %s function", dlu_msg);
      break;
    case DLU_VK_FUNC_ERR:
      dlu_log_me(DLU_DANGER, "[x] %s: %s", dlu_msg, vkres_msg(vkerr));
      break;
    case DLU_MEM_TYPE_ERR:
      dlu_log_me(DLU_DANGER, "[x] memory_type_from_properties failed");
      break;
    case DLU_VKCOMP_INSTANCE:
      dlu_log_me(DLU_DANGER, "[x] A Vulkan Instance must be created");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_instance()");
      break;
    case DLU_VKCOMP_DEVICE:
      dlu_log_me(DLU_DANGER, "[x] A logical device must be initialize");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_logical_device()");
      break;
    case DLU_VKCOMP_RENDER_PASS:
      dlu_log_me(DLU_DANGER, "[x] render pass not setup");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_render_pass()");
      break;
    case DLU_VKCOMP_PIPELINE_LAYOUT:
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_pipeline_layout()");
      break;
    case DLU_VKCOMP_DESC_POOL:
      dlu_log_me(DLU_DANGER, "[x] In order to allocate descriptor sets one must have a descriptor pool");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_desc_pool()");
      break;
    case DLU_VKCOMP_DESC_LAYOUT:
      dlu_log_me(DLU_DANGER, "[x] In order to allocate descriptor sets one must define a descriptor layout");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_desc_set_layouts()");
      break;
    case DLU_VKCOMP_PHYS_DEV:
      dlu_log_me(DLU_DANGER, "[x] A VkPhysical device must be set");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_physical_device()");
      break;
    case DLU_VKCOMP_SURFACE:
      dlu_log_me(DLU_DANGER, "[x] A VkSurfaceKHR must be initialize");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_vkwayland_surfaceKHR()");
      break;
    case DLU_VKCOMP_SC:
      dlu_log_me(DLU_DANGER, "[x] Swap Chain doesn't exists");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_swap_chain()");
      break;
    case DLU_VKCOMP_SC_IC:
      dlu_log_me(DLU_DANGER, "[x] Swapchain image count not set");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_swap_chain()");
      break;
    case DLU_VKCOMP_SC_SYNCS:
      dlu_log_me(DLU_DANGER, "[x] Swapchain sychronizers must be initialize before use");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_syncs()");
      break;
    case DLU_VKCOMP_BUFF_MEM:
      dlu_log_me(DLU_DANGER, "[x] VkDeviceMemory not created");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_vk_buffer() or dlu_create_texture_image()");
      break;
    case DLU_VKCOMP_CMD_POOL:
      dlu_log_me(DLU_DANGER, "[x] In order to allocate command buffers one must have a command pool");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_cmd_pool()");
      break;
    case DLU_VKCOMP_CMD_BUFFS:
      dlu_log_me(DLU_DANGER, "[x] VkCommandBuffer buffers must be initialize");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_create_cmd_buffs()");
      break;
    case DLU_VKCOMP_DEVICE_NOT_ASSOC:
      dlu_log_me(DLU_DANGER, "[x] Must have a VkDevice or a VkPhysicalDevice association");
      dlu_log_me(DLU_DANGER, "[x] Must make a call to %s to create that association", dlu_msg);
      break;
    case DLU_BUFF_NOT_ALLOC:
      dlu_log_me(DLU_DANGER, "[x] Must make a call to dlu_otba(): %s", dlu_msg);
      break;
    case DLU_OP_NOT_PERMITED:
      dlu_log_me(DLU_DANGER, "[x] Failure the current operation you are trying to do is wrong");
      break;
    case DLU_ALLOC_FAILED:
      dlu_log_me(DLU_DANGER, "[x] Ugh something went wrong!! :( If you didn't forget to call dlu_otma(3). You might of under allocated");
      break;
    case DLU_ALREADY_ALLOC:
      dlu_log_me(DLU_DANGER, "[x] Buffer already allocated, Don't try again >>[]:");
      break;
    default: break;
  }
}
