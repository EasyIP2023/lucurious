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

#ifndef DLU_VKCOMP_DISPLAY_FUNCS_H
#define DLU_VKCOMP_DISPLAY_FUNCS_H

/**
* Needed to create the swap chain
* This function queries your physical device's surface capabilities.
* Mainly used to get minImageCount and the extent/resolution
*/
VkSurfaceCapabilitiesKHR dlu_get_physical_device_surface_capabilities(vkcomp *app, uint32_t cur_pd);

/**
* Needed to create the swap chain
* Function returns the best format and the color depth for an image(s),
* but gives the option to select a preference.
* The "format" variable refers to the pixel formats (width of each texels color space in memory)
* and the "colorSpace" variable refers to the Color Depth
*/
VkSurfaceFormatKHR dlu_choose_swap_surface_format(vkcomp *app, uint32_t cur_pd, VkFormat format, VkColorSpaceKHR colorSpace);

/**
* Needed to create the swap chain
* This function chooses the best presentation mode for the swapchain/presentation engine.
* Conditions required for swapping images to the screen.
* Basically determines if an application should be waiting for v-sync or display
* an image immediately (definitely leads to image tearing). 
*/
VkPresentModeKHR dlu_choose_swap_present_mode(vkcomp *app, uint32_t cur_pd);

/**
* Needed to create the swap chain
* Function returns the best resolution for the images in the swap chain
*/
VkExtent2D dlu_choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height);

/**
* Acquire the swapchain image in order to set its layout
* cur_scd: Current Swapchain Data Members. Will auto choose current swapchain and synchronization members
* cur_sync: Current Synchronization Members. Will auto select image semaphore to signal (vkcomp->sc_data->syncs)
* cur_img: Allows for vkAcquireNextImageKHR to set the image index
*/
VkResult dlu_acquire_sc_image_index(vkcomp *app, uint32_t cur_scd, uint32_t cur_sync, uint32_t *cur_img);

/**
* Puts command buffers into the graphics queue, to be received and processed by the GPU
* cur_scd: Current swap chain struct index data member
* sycni: Represents the current vkcomp->sc_data->synchronize struct index
*/
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
);

/* Submit results back to the swap chain, to be presented on the screen */
VkResult dlu_queue_present_queue(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  uint32_t swapchainCount,
  const VkSwapchainKHR *pSwapchains,
  const uint32_t *pImageIndices,
  VkResult *pResults
);

/**
* Query display properities for a choosen physical device to allow for
* implementation of a rendering/presentation backend of a windowing system
*/
VkResult dlu_get_physical_device_display_propertiesKHR(vkcomp *app, uint32_t cur_pd);

#endif
