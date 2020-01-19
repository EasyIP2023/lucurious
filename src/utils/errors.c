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

void _show_err_msg(uint32_t wlu_err, int vkerr, const char *wlu_msg) {
  switch (wlu_err) {
    case WLU_DR_INSTANCE_PROC_ADDR_ERR:
      wlu_log_me(WLU_DANGER, "[x] GetInstanceProcAddr: Unable to find %s function", wlu_msg);
      break;
    case WLU_DR_DEVICE_PROC_ADDR_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkGetDeviceProcAddr: Unable to find %s function", wlu_msg);
      break;
    case WLU_VK_ENUM_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkEnumerate%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_CREATE_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkCreate%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_ALLOC_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkAllocate%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_BEGIN_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkBegin%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_END_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkEnd%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_QUEUE_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkQueue%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_GET_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkGet%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_BIND_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkBind%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_MAP_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkMap%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_FLUSH_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkFlush%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_RESET_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkReset%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VK_WAIT_ERR:
      wlu_log_me(WLU_DANGER, "[x] vkWait%s: %s", wlu_msg, vkres_msg(vkerr));
      break;
    case WLU_VKCOMP_INSTANCE:
      wlu_log_me(WLU_DANGER, "[x] A Vulkan Instance must be created");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_instance()");
      break;
    case WLU_VKCOMP_DEVICE:
      wlu_log_me(WLU_DANGER, "[x] A logical device must be initialize");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device()");
      break;
    case WLU_VKCOMP_RENDER_PASS:
      wlu_log_me(WLU_DANGER, "[x] render pass not setup");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_render_pass()");
      break;
    case WLU_VKCOMP_PIPELINE_LAYOUT:
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_pipeline_layout()");
      break;
    case WLU_VKCOMP_DESC_POOL:
      wlu_log_me(WLU_DANGER, "[x] In order to allocate descriptor sets one must have a descriptor pool");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_desc_pool()");
      break;
    case WLU_VKCOMP_DESC_LAYOUT:
      wlu_log_me(WLU_DANGER, "[x] In order to allocate descriptor sets one must define a descriptor layout");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_desc_set_layouts()");
      break;
    case WLU_VKCOMP_FRAMEBUFFER:
      wlu_log_me(WLU_DANGER, "[x] Frame Buffers weren't created");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_framebuffers()");
      break;
    case WLU_VKCOMP_PHYSICAL_DEV:
      wlu_log_me(WLU_DANGER, "[x] A VkPhysical device must be set");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_physical_device()");
      break;
    case WLU_VKCOMP_INDICES:
      wlu_log_me(WLU_DANGER, "[x] At least one queue family should be set");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_queue_family(3)");
      break;
    case WLU_VKCOMP_SURFACE:
      wlu_log_me(WLU_DANGER, "[x] A VkSurfaceKHR must be initialize");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR()");
      break;
    case WLU_VKCOMP_SC:
      wlu_log_me(WLU_DANGER, "[x] Swap Chain doesn't exists");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_swap_chain()");
      break;
    case WLU_VKCOMP_SC_BUFFS:
      wlu_log_me(WLU_DANGER, "[x] Swap Chain buffers not setup");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_img_views()");
      break;
    case WLU_VKCOMP_SC_IC:
      wlu_log_me(WLU_DANGER, "[x] Swapchain image count not set");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_swap_chain()");
      break;
    case WLU_VKCOMP_SC_SEMS:
      wlu_log_me(WLU_DANGER, "[x] Image semaphores must be initialize before use");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_semaphores()");
      break;
    case WLU_VKCOMP_BUFF_MEM:
      wlu_log_me(WLU_DANGER, "[x] VkDeviceMemory not created");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_buffer()");
      break;
    case WLU_VKCOMP_CMD_POOL:
      wlu_log_me(WLU_DANGER, "[x] In order to allocate command buffers one must have a command pool");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_cmd_pool()");
      break;
    case WLU_VKCOMP_CMD_BUFFS:
      wlu_log_me(WLU_DANGER, "[x] VkCommandBuffer buffers must be initialize");
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_cmd_buffs()");
      break;
    case WLU_BUFF_NOT_ALLOC:
      wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_otba(): %s", wlu_msg);
      break;
    case WLU_ALLOC_FAILED:
      wlu_log_me(WLU_DANGER, "[x] something seriously went wrong can't find sub range of addresses, please create issue with information");
      break;
    default: break;
  }
}
