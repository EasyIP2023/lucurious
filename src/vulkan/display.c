/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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

VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->instance) {
    wlu_log_me(WLU_DANGER, "[x] A VkInstance must be established");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_instance(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = (struct wl_display *) wl_display;
  create_info.surface = (struct wl_surface *) wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  return res;
}

VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace) {
  VkResult err;
  VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_format;
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, NULL);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d", err);
    goto finish_format;
  }

  if (format_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device surface formats, format_count equals 0");
    goto finish_format;
  }

  formats = (VkSurfaceFormatKHR *) calloc(sizeof(VkSurfaceFormatKHR),
    format_count * sizeof(VkSurfaceFormatKHR));
  if (!formats) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkSurfaceFormatKHR *formats failed");
    goto finish_format;
  }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, formats);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfaceFormatsKHR failed, ERROR CODE: %d", err);
    goto finish_format;
  }

  ret_fmt = formats[0];

  if (format_count == 1 && formats[0].format == format) {
    ret_fmt.format = format;
    ret_fmt.colorSpace = colorSpace;
    goto finish_format;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == format && formats[i].colorSpace == colorSpace) {
      ret_fmt = formats[i];
      goto finish_format;
    }
  }

finish_format:
  if (formats) {
    free(formats);
    formats = NULL;
  }
  return ret_fmt;
}

VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_best_mode;
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, NULL);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d", err);
    goto finish_best_mode;
  }

  if (pres_mode_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device presentation modes, pres_mode_count equals 0");
    goto finish_best_mode;
  }

  present_modes = (VkPresentModeKHR *) calloc(sizeof(VkPresentModeKHR),
      pres_mode_count * sizeof(VkPresentModeKHR));

  if (!present_modes) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkPresentModeKHR *present_modes failed");
    goto finish_best_mode;
  }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, present_modes);
  if (err) {
    wlu_log_me(WLU_DANGER, "[x] vkGetPhysicalDeviceSurfacePresentModesKHR failed, ERROR CODE: %d", err);
    goto finish_best_mode;
  }

  /* Only mode that is guaranteed */
  best_mode = VK_PRESENT_MODE_FIFO_KHR;

  for (uint32_t i = 0; i < pres_mode_count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      best_mode = present_modes[i];
      /* For triple buffering */
      goto finish_best_mode;
    }
    else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      best_mode = present_modes[i];
    }
  }

finish_best_mode:
  if (present_modes) {
    free(present_modes);
    present_modes = NULL;
  }
  return best_mode;
}

VkExtent2D wlu_choose_2D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = {width, height};

    actual_extent.width = max(capabilities.minImageExtent.width,
                          min(capabilities.maxImageExtent.width,
                          actual_extent.width));
    actual_extent.height = max(capabilities.minImageExtent.height,
                           min(capabilities.maxImageExtent.height,
                           actual_extent.height));
    return actual_extent;
  }
}

VkExtent3D wlu_choose_3D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height, uint32_t depth) {
  VkExtent3D actual_extent = {width, height, depth};
  actual_extent.width = max(capabilities.minImageExtent.width,
                        min(capabilities.maxImageExtent.width,
                        actual_extent.width));
  actual_extent.height = max(capabilities.minImageExtent.height,
                         min(capabilities.maxImageExtent.height,
                         actual_extent.height));
  return actual_extent;
}

VkResult wlu_exec_begin_cmd_buff(
  vkcomp *app,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->cmd_buffs) {
    wlu_log_me(WLU_DANGER, "[x] app->cmd_buffs must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_cmd_buffs(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  for (uint32_t i = 0; i < app->sc_buff_size; i++) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = NULL;
    begin_info.flags = flags;
    begin_info.pInheritanceInfo = pInheritanceInfo;

    res = vkBeginCommandBuffer(app->cmd_buffs[i], &begin_info);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] Failed to start recording command buffer [%d], ERROR CODE: %d", i, res);
      return res;
    }
  }

  return res;
}

VkResult wlu_exec_stop_cmd_buff(vkcomp *app) {
  VkResult res = VK_RESULT_MAX_ENUM;

  for (uint32_t i = 0; i < app->sc_buff_size; i++) {
    res = vkEndCommandBuffer(app->cmd_buffs[i]);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] Failed to stop recording command buffer [%d], ERROR CODE: %d", i, res);
      return res;
    }
  }

  return res;
}

VkResult wlu_draw_frame(
  vkcomp *app,
  uint32_t *image_index,
  uint32_t waitSemaphoreCount,
  VkSemaphore *pWaitSemaphores,
  VkPipelineStageFlags *pWaitDstStageMask,
  uint32_t commandBufferCount,
  uint32_t signalSemaphoreCount,
  VkSemaphore *pSignalSemaphores,
  uint32_t swapchainCount,
  VkSwapchainKHR *pSwapchains,
  VkResult *pResults
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->img_semaphore) {
    wlu_log_me(WLU_DANGER, "[x] Image semaphore must be initialize before use");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_semaphores(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  /* UINT64_MAX disables timeout */
  res = vkAcquireNextImageKHR(app->device, app->swap_chain, UINT64_MAX,
                              app->img_semaphore, VK_NULL_HANDLE, image_index);

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = NULL;
  submit_info.waitSemaphoreCount = waitSemaphoreCount;
  submit_info.pWaitSemaphores = pWaitSemaphores;
  submit_info.pWaitDstStageMask = pWaitDstStageMask;
  submit_info.commandBufferCount = commandBufferCount;
  submit_info.pCommandBuffers = &app->cmd_buffs[*image_index];
  submit_info.signalSemaphoreCount = signalSemaphoreCount;
  submit_info.pSignalSemaphores = pSignalSemaphores;

  res = vkQueueSubmit(app->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  if (res) return res;

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = NULL;
  present_info.waitSemaphoreCount = waitSemaphoreCount;
  present_info.pWaitSemaphores = pWaitSemaphores;
  present_info.swapchainCount = swapchainCount;
  present_info.pSwapchains = pSwapchains;
  present_info.pImageIndices = image_index;
  present_info.pResults = pResults;

  res = vkQueuePresentKHR(app->present_queue, &present_info);

  return res;
}
