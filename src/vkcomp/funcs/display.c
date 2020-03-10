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

VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->instance) { PERR(WLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = (struct wl_display *) wl_display;
  create_info.surface = (struct wl_surface *) wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  if (res) PERR(WLU_VK_CREATE_ERR, res, "WaylandSurfaceKHR")

  return res;
}

VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace) {
  VkResult err;
  VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  if (!app->surface) { PERR(WLU_VKCOMP_SURFACE, 0, NULL); return ret_fmt; }
  if (!app->physical_device) { PERR(WLU_VKCOMP_PHYS_DEV, 0, NULL);  return ret_fmt; }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, NULL);
  if (err) { PERR(WLU_VK_GET_ERR, err, "PhysicalDeviceSurfaceFormatsKHR"); return ret_fmt; }

  if (format_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device surface formats");
    return ret_fmt;
  }

  formats = (VkSurfaceFormatKHR *) alloca(format_count * sizeof(VkSurfaceFormatKHR));

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->physical_device, app->surface, &format_count, formats);
  if (err) { PERR(WLU_VK_GET_ERR, err, "PhysicalDeviceSurfaceFormatsKHR"); return ret_fmt; }

  ret_fmt = formats[0];

  if (format_count == 1 && formats[0].format == format) {
    ret_fmt.format = format;
    ret_fmt.colorSpace = colorSpace;
    return ret_fmt;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == format && formats[i].colorSpace == colorSpace) {
      ret_fmt = formats[i];
      return ret_fmt;
    }
  }

  return ret_fmt;
}

VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  if (!app->surface) { PERR(WLU_VKCOMP_SURFACE, 0, NULL); return best_mode; }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, NULL);
  if (err) { PERR(WLU_VK_GET_ERR, err, "PhysicalDeviceSurfacePresentModesKHR"); return best_mode; }

  if (pres_mode_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] Failed to find Physical Device presentation modes");
    return best_mode;
  }

  present_modes = (VkPresentModeKHR *) alloca(pres_mode_count * sizeof(VkPresentModeKHR));

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->physical_device, app->surface, &pres_mode_count, present_modes);
  if (err) { PERR(WLU_VK_GET_ERR, err, "PhysicalDeviceSurfacePresentModesKHR"); return best_mode; }

  /* Only mode that is guaranteed */
  best_mode = VK_PRESENT_MODE_FIFO_KHR;

  for (uint32_t i = 0; i < pres_mode_count; i++) {
    if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) { /* For triple buffering */
      best_mode = present_modes[i];
      break;
    }
    else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      best_mode = present_modes[i];
    }
  }

  return best_mode;
}

VkExtent2D wlu_choose_2D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = {width, height};

    actual_extent.width = fmax(capabilities.minImageExtent.width,
                          fmin(capabilities.maxImageExtent.width,
                               actual_extent.width));
    actual_extent.height = fmax(capabilities.minImageExtent.height,
                           fmin(capabilities.maxImageExtent.height,
                                actual_extent.height));
    return actual_extent;
  }
}

VkExtent3D wlu_choose_3D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height, uint32_t depth) {
  VkExtent3D actual_extent = {width, height, depth};
  actual_extent.width = fmax(capabilities.minImageExtent.width,
                        fmin(capabilities.maxImageExtent.width,
                             actual_extent.width));
  actual_extent.height = fmax(capabilities.minImageExtent.height,
                         fmin(capabilities.maxImageExtent.height,
                              actual_extent.height));
  return actual_extent;
}

/**
* Vince: KEEP COMMENT FOR FUTURE REFERENCE
* https://www.reddit.com/r/vulkan/comments/5vzijc/question_about_vksemaphore_usage/
*/
VkResult wlu_acquire_sc_image_index(vkcomp *app, uint32_t cur_scd, uint32_t *cur_img) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->sc_data[cur_scd].sems) { PERR(WLU_VKCOMP_SC_SEMS, 0, NULL); return res; }

  /* Signal image semaphore */
  res = vkAcquireNextImageKHR(app->device, app->sc_data[cur_scd].swap_chain, GENERAL_TIMEOUT,
                              app->sc_data[cur_scd].sems[*cur_img].image, VK_NULL_HANDLE, cur_img);
  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    wlu_freeup_sc(app);
    return res;
  }

  return res;
}

VkResult wlu_queue_graphics_queue(
  vkcomp *app,
  uint32_t commandBufferCount,
  VkCommandBuffer *pCommandBuffers,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  const VkPipelineStageFlags *pWaitDstStageMask,
  uint32_t signalSemaphoreCount,
  const VkSemaphore *pSignalSemaphores
) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkFence draw_fence = VK_NULL_HANDLE;

  /* Queue the command buffer for execution */
  VkFenceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  /* Use fence to synchronize application with rendering operation */
  res = vkCreateFence(app->device, &create_info, NULL, &draw_fence);
  if (res) { PERR(WLU_VK_CREATE_ERR, res, "Fence"); goto finish_gq_submit; }

  VkSubmitInfo submit_info = {};
  submit_info.pNext = NULL;
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = waitSemaphoreCount;
  submit_info.pWaitSemaphores = pWaitSemaphores;
  submit_info.pWaitDstStageMask = pWaitDstStageMask;
  submit_info.commandBufferCount = commandBufferCount;
  submit_info.pCommandBuffers = pCommandBuffers;
  submit_info.signalSemaphoreCount = signalSemaphoreCount;
  submit_info.pSignalSemaphores = pSignalSemaphores;

  do {
    /* set fence to unsignaled state */
    res = vkResetFences(app->device, 1, &draw_fence);
    if (res) { PERR(WLU_VK_RESET_ERR, res, "Fences"); goto finish_gq_submit; }
	
    /* Semaphore should be waited on & fence should be signaled when command buffer is finished */
    res = vkQueueSubmit(app->graphics_queue, 1, &submit_info, draw_fence);
    if (res) { PERR(WLU_VK_QUEUE_ERR, res, "Submit"); goto finish_gq_submit; }

    /* Use fence to synchronize application with rendering operation */
    res = vkWaitForFences(app->device, 1, &draw_fence, VK_TRUE, GENERAL_TIMEOUT);
    if (res) { PERR(WLU_VK_WAIT_ERR, res, "ForFences"); goto finish_gq_submit; }

  } while (res == VK_TIMEOUT);

finish_gq_submit:
  if (draw_fence) {
    vkDestroyFence(app->device, draw_fence, NULL);
    draw_fence = VK_NULL_HANDLE;
  }
  return res;
}

VkResult wlu_queue_present_queue(
  vkcomp *app,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  uint32_t swapchainCount,
  const VkSwapchainKHR *pSwapchains,
  const uint32_t *pImageIndices,
  VkResult *pResults
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkPresentInfoKHR present;
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.pNext = NULL;
  present.waitSemaphoreCount = waitSemaphoreCount;
  present.pWaitSemaphores = pWaitSemaphores;
  present.swapchainCount = swapchainCount;
  present.pSwapchains = pSwapchains;
  present.pImageIndices = pImageIndices;
  present.pResults = pResults;

  res = vkQueuePresentKHR(app->present_queue, &present);
  if (res) { PERR(WLU_VK_QUEUE_ERR, res, "PresentKHR"); return res; }

  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    wlu_freeup_sc(app);

  /* Wait for operations in present queue to finish */
  vkQueueWaitIdle(app->present_queue);

  return res;
}
