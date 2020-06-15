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
* alloca()'s usage here is meant for stack space efficiency
* Fixed size arrays tend to over allocate, while alloca will
* allocate the exact amount of bytes that you want
*/

VkSurfaceCapabilitiesKHR dlu_get_physical_device_surface_capabilities(vkcomp *app, uint32_t cur_pd) {
  VkResult err;
  VkSurfaceCapabilitiesKHR capabilities;
  capabilities.minImageCount = UINT32_MAX;

  if (!app->pd_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_PD_DATA"); return capabilities; }
  if (!app->surface) { PERR(DLU_VKCOMP_SURFACE, 0, NULL); return capabilities; }
  if (!app->pd_data[cur_pd].phys_dev) { PERR(DLU_VKCOMP_PHYS_DEV, 0, NULL); return capabilities; }

  /* Not going to check if physical device present user should know by now */
  err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(app->pd_data[cur_pd].phys_dev, app->surface, &capabilities);
  if (err) {
    PERR(DLU_VK_FUNC_ERR, err, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    return capabilities;
  }

  return capabilities;
}

VkSurfaceFormatKHR dlu_choose_swap_surface_format(vkcomp *app, uint32_t cur_pd, VkFormat format, VkColorSpaceKHR colorSpace) {
  VkResult err;
  VkSurfaceFormatKHR ret_fmt = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
  VkSurfaceFormatKHR *formats = VK_NULL_HANDLE;
  uint32_t format_count = 0;

  if (!app->pd_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_PD_DATA"); return ret_fmt; }
  if (!app->surface) { PERR(DLU_VKCOMP_SURFACE, 0, NULL); return ret_fmt; }
  if (!app->pd_data[cur_pd].phys_dev) { PERR(DLU_VKCOMP_PHYS_DEV, 0, NULL); return ret_fmt; }

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->pd_data[cur_pd].phys_dev, app->surface, &format_count, NULL);
  if (err) { PERR(DLU_VK_FUNC_ERR, err, "vkGetPhysicalDeviceSurfaceFormatsKHR"); return ret_fmt; }

  if (format_count == 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to find Physical Device surface formats");
    return ret_fmt;
  }

  formats = (VkSurfaceFormatKHR *) alloca(format_count * sizeof(VkSurfaceFormatKHR));

  err = vkGetPhysicalDeviceSurfaceFormatsKHR(app->pd_data[cur_pd].phys_dev, app->surface, &format_count, formats);
  if (err) { PERR(DLU_VK_FUNC_ERR, err, "vkGetPhysicalDeviceSurfaceFormatsKHR"); return ret_fmt; }

  ret_fmt = formats[0];

  if (format_count == 1 && formats[0].format == format) {
    ret_fmt.format = format;
    ret_fmt.colorSpace = colorSpace;
    return ret_fmt;
  }

  for (uint32_t i = 0; i < format_count; i++) {
    if (formats[i].format == format && formats[i].colorSpace == colorSpace) {
      ret_fmt = formats[i];
      break;
    }
  }

  return ret_fmt;
}

VkPresentModeKHR dlu_choose_swap_present_mode(vkcomp *app, uint32_t cur_pd) {
  VkResult err;
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  VkPresentModeKHR *present_modes = VK_NULL_HANDLE;
  uint32_t pres_mode_count = 0;

  if (!app->pd_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_PD_DATA"); return best_mode; }
  if (!app->surface) { PERR(DLU_VKCOMP_SURFACE, 0, NULL); return best_mode; }
  if (!app->pd_data[cur_pd].phys_dev) { PERR(DLU_VKCOMP_PHYS_DEV, 0, NULL); return best_mode; }

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->pd_data[cur_pd].phys_dev, app->surface, &pres_mode_count, NULL);
  if (err) { PERR(DLU_VK_FUNC_ERR, err, "vkGetPhysicalDeviceSurfacePresentModesKHR"); return best_mode; }

  if (pres_mode_count == 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to find Physical Device presentation modes");
    return best_mode;
  }

  present_modes = (VkPresentModeKHR *) alloca(pres_mode_count * sizeof(VkPresentModeKHR));

  err = vkGetPhysicalDeviceSurfacePresentModesKHR(app->pd_data[cur_pd].phys_dev, app->surface, &pres_mode_count, present_modes);
  if (err) { PERR(DLU_VK_FUNC_ERR, err, "vkGetPhysicalDeviceSurfacePresentModesKHR"); return best_mode; }

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

/**
* If the width retrieved from making a call to dlu_get_physical_device_surface_capabilities()
* doesn't equal -1 then leave VkExtent2D struct in VkSurfaceCapabilitiesKHR struct the same
* else find a suitable replacement
*/
VkExtent2D dlu_choose_swap_extent(VkSurfaceCapabilitiesKHR cap, uint32_t width, uint32_t height) {
  VkExtent2D extent = {width, height};

  if (cap.currentExtent.width != UINT32_MAX) {
    extent = cap.currentExtent;
  } else {
    extent.width = fmax(cap.minImageExtent.width, fmin(cap.maxImageExtent.width, extent.width));
    extent.height = fmax(cap.minImageExtent.height, fmin(cap.maxImageExtent.height, extent.height)); 
  }

  return extent;
}

VkResult dlu_acquire_sc_image_index(vkcomp *app, uint32_t cur_scd, uint32_t cur_sync, uint32_t *cur_img) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->sc_data[cur_scd].syncs) { PERR(DLU_VKCOMP_SC_SYNCS, 0, NULL); return res; }

  /* Signal image semaphore */
  res = vkAcquireNextImageKHR(app->ld_data[app->sc_data[cur_scd].ldi].device, app->sc_data[cur_scd].swap_chain,
                              GENERAL_TIMEOUT, app->sc_data[cur_scd].syncs[cur_sync].sem.image, VK_NULL_HANDLE, cur_img);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkAcquireNextImageKHR")

  return res;
}

VkResult dlu_queue_graphics_queue(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t synci,
  uint32_t commandBufferCount,
  VkCommandBuffer *pCommandBuffers,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  const VkPipelineStageFlags *pWaitDstStageMask,
  uint32_t signalSemaphoreCount,
  const VkSemaphore *pSignalSemaphores
) {

  VkResult res = VK_RESULT_MAX_ENUM;

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

  /**
  * Fence will be in signaled state when the command buffers finish execution
  * VkFence render: Used to signal that a frame has finished rendering
  */
  res = vkQueueSubmit(app->ld_data[app->sc_data[cur_scd].ldi].graphics, 1, &submit_info, app->sc_data[cur_scd].syncs[synci].fence.render);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkQueueSubmit")

  return res;
}

VkResult dlu_queue_present_queue(
  vkcomp *app,
  uint32_t cur_ld,
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

  res = vkQueuePresentKHR(app->ld_data[cur_ld].present, &present);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkQueuePresentKHR")

  return res;
}

VkResult dlu_get_physical_device_display_propertiesKHR(vkcomp *app, uint32_t cur_pd) {

  VkResult res = VK_RESULT_MAX_ENUM;
  VkDisplayPropertiesKHR *pProperties = NULL;

  if (app->dis_data) { dlu_log_me(DLU_DANGER, "[x] dlu_get_physical_device_display_propertiesKHR: can only run once"); return res; }

  res = vkGetPhysicalDeviceDisplayPropertiesKHR(app->pd_data[cur_pd].phys_dev, &app->dpc, NULL);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkGetPhysicalDeviceDisplayPropertiesKHR"); return res; }

  if (!app->dpc) { dlu_log_me(DLU_DANGER, "[x] vkGetPhysicalDeviceDisplayPropertiesKHR: pPropertyCount = 0"); return res; }

  pProperties = alloca(app->dpc * sizeof(VkDisplayPropertiesKHR));

  res = vkGetPhysicalDeviceDisplayPropertiesKHR(app->pd_data[cur_pd].phys_dev, &app->dpc, pProperties);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkGetPhysicalDeviceDisplayPropertiesKHR");

  /* Allocate and Assign */
  res = dlu_otba(DLU_DIS_DATA, app, INDEX_IGNORE, app->dpc);
  if (res) { PERR(DLU_ALLOC_FAILED, 0, NULL); return res; }

  for (uint32_t i = 0; i < app->dpc; i++)
    app->dis_data[i].props = pProperties[i];

  return res;
}
