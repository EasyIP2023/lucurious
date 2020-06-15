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

VkResult dlu_vk_sync(dlu_sync_type type, vkcomp *app, uint32_t cur_scd, uint32_t synci) {
  VkResult res = VK_RESULT_MAX_ENUM;

  switch (type) {
    case DLU_VK_WAIT_IMAGE_FENCE: /* set render fence to signal state */
      res = vkWaitForFences(app->ld_data[app->sc_data[cur_scd].ldi].device, 1, &app->sc_data[cur_scd].syncs[synci].fence.image, VK_TRUE, GENERAL_TIMEOUT);
      if (res) PERR(DLU_VK_FUNC_ERR, res, "vkWaitForFences")
      break;
    case DLU_VK_WAIT_RENDER_FENCE: /* set image fence to signal state */
      res = vkWaitForFences(app->ld_data[app->sc_data[cur_scd].ldi].device, 1, &app->sc_data[cur_scd].syncs[synci].fence.render, VK_TRUE, GENERAL_TIMEOUT);
      if (res) PERR(DLU_VK_FUNC_ERR, res, "vkWaitForFences")
      break;
    case DLU_VK_WAIT_PRESENT_QUEUE:
      res = vkQueueWaitIdle(app->ld_data[app->sc_data[cur_scd].ldi].present);
      if (res) PERR(DLU_VK_FUNC_ERR, res, "vkQueueWaitIdle")
      break;
    case DLU_VK_WAIT_GRAPHICS_QUEUE:
      res = vkQueueWaitIdle(app->ld_data[app->sc_data[cur_scd].ldi].graphics);
      if (res) PERR(DLU_VK_FUNC_ERR, res, "vkQueueWaitIdle") 
      break;
    case DLU_VK_RESET_RENDER_FENCE: /* set fence to unsignaled state */
      res = vkResetFences(app->ld_data[app->sc_data[cur_scd].ldi].device, 1, &app->sc_data[cur_scd].syncs[synci].fence.render);
      if (res) PERR(DLU_VK_FUNC_ERR, res, "vkResetFences")
      break;
    case DLU_VK_GET_RENDER_FENCE:
      res = vkGetFenceStatus(app->ld_data[app->sc_data[cur_scd].ldi].device, app->sc_data[cur_scd].syncs[synci].fence.render);
      switch(res) {
        case VK_SUCCESS:
          dlu_log_me(DLU_WARNING, "The fence specified app->sc_data[%d].syncs[%d].fence.render is signaled.", cur_scd, synci);
          break;
        case VK_NOT_READY:
          dlu_log_me(DLU_WARNING, "The fence specified app->sc_data[%d].syncs[%d].fence.render is unsignaled.", cur_scd, synci);
          break;
        case VK_ERROR_DEVICE_LOST:
          dlu_log_me(DLU_WARNING, "The device has been lost.");
          break;
        default: break;
      }
    break;
    default: break;
  }

  return res;
}

void dlu_vk_destroy(dlu_destroy_type type, vkcomp *app, uint32_t cur_ld, void *data) {
  switch (type) {
      case DLU_DESTROY_VK_SHADER:
        {VkShaderModule shader_module = (VkShaderModule) data;
         if (shader_module) vkDestroyShaderModule(app->ld_data[cur_ld].device, shader_module, NULL);}
        break;
      case DLU_DESTROY_VK_BUFFER:
        {VkBuffer buff = (VkBuffer) data;
         if (buff) vkDestroyBuffer(app->ld_data[cur_ld].device, buff, NULL);}
        break;
      case DLU_DESTROY_VK_MEMORY:
        {VkDeviceMemory mem = (VkDeviceMemory) data;
         if (mem) vkFreeMemory(app->ld_data[cur_ld].device, mem, NULL);}
        break;
      case DLU_DESTROY_VK_CMD_POOL:
        {VkCommandPool pool = (VkCommandPool) data; 
         if (pool) vkDestroyCommandPool(app->ld_data[cur_ld].device, pool, NULL);}
        break;
      case DLU_DESTROY_VK_DESC_POOL:
        {VkDescriptorPool pool = (VkDescriptorPool) data; 
         if (pool) vkDestroyDescriptorPool(app->ld_data[cur_ld].device, pool, NULL);}
        break;
      case DLU_DESTROY_VK_DESC_SET_LAYOUT:
        {VkDescriptorSetLayout layout = (VkDescriptorSetLayout) data;
         if (layout) vkDestroyDescriptorSetLayout(app->ld_data[cur_ld].device, layout, NULL);}
        break;
      case DLU_DESTROY_PIPELINE_CACHE:
        {VkPipelineCache cache = (VkPipelineCache) data;
         if (cache) vkDestroyPipelineCache(app->ld_data[cur_ld].device, cache, NULL);}
        break;
      case DLU_DESTROY_VK_FRAME_BUFFER:
        {VkFramebuffer frame = (VkFramebuffer) data;
         if (frame) vkDestroyFramebuffer(app->ld_data[cur_ld].device, frame, NULL);}
        break;
      case DLU_DESTROY_VK_RENDER_PASS:
        {VkRenderPass rp = (VkRenderPass) data;
         if (rp) vkDestroyRenderPass(app->ld_data[cur_ld].device, rp, NULL);}
        break;
      case DLU_DESTROY_VK_PIPE_LAYOUT:
        {VkPipelineLayout pipe_layout = (VkPipelineLayout) data;
         if (pipe_layout) vkDestroyPipelineLayout(app->ld_data[cur_ld].device, pipe_layout, NULL);}
        break;
      case DLU_DESTROY_PIPELINE:
        {VkPipeline pipeline = (VkPipeline) data;
         if (pipeline) vkDestroyPipeline(app->ld_data[cur_ld].device, pipeline, NULL);}
        break;
      case DLU_DESTROY_VK_SAMPLER:
        {VkSampler sampler = (VkSampler) data;
         if (sampler) vkDestroySampler(app->ld_data[cur_ld].device, sampler, NULL);}
        break;
      case DLU_DESTROY_VK_IMAGE:
        {VkImage image = (VkImage) data;
         if (image) vkDestroyImage(app->ld_data[cur_ld].device, image, NULL);}
        break;
      case DLU_DESTROY_VK_IMAGE_VIEW:
        {VkImageView view = (VkImageView) data;
         if (view) vkDestroyImageView(app->ld_data[cur_ld].device, view, NULL);}
        break;
      case DLU_DESTROY_VK_SWAPCHAIN:
        {VkSwapchainKHR swapchain = (VkSwapchainKHR) data;
         if (swapchain) vkDestroySwapchainKHR(app->ld_data[cur_ld].device, swapchain, NULL);}
        break;
      case DLU_DESTROY_VK_SEMAPHORE:
        {VkSemaphore semaphore = (VkSemaphore) data;
         if (semaphore) vkDestroySemaphore(app->ld_data[cur_ld].device, semaphore, NULL);}
        break;
      case DLU_DESTROY_VK_FENCE:
        {VkFence fence = (VkFence) data;
         if (fence) vkDestroyFence(app->ld_data[cur_ld].device, fence, NULL);}
        break;
      case DLU_DESTROY_VK_LOGIC_DEVICE:
         if (app->ld_data[cur_ld].device) vkDestroyDevice(app->ld_data[cur_ld].device, NULL);
        break;
      default: break;
  }
}
