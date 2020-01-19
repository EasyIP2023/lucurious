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

#ifndef WLU_VKCOMP_DISPLAY_FUNCS_H
#define WLU_VKCOMP_DISPLAY_FUNCS_H

/**
* How Vulkan establishes connection with window system.
* Through the use of Window System Integration (WSI).
* This fuction exposes a VkSurfaceKHR object that represents
* a surface to present rendered images to.
*/
VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface);

/**
* Needed to create the swap chain. This will specify the format and
* the surace of an image. The "format" variable refers to the pixel
* formats and the "colorSpace" variable refers to the Color Depth
*/
VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace);

/**
* Needed to create the swap chain
* This function chooses the best presentation mode for swapchain
* (Conditions required for swapping images to the screen)
*/
VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app);

/**
* Needed to create the swap chain. Pick the resolution
* for the images in the swap chain
*/
VkExtent2D wlu_choose_2D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height);

/**
* Needed to create the swap chain. Pick the resolution
* for the images in the swap chain along with the depth
* of the 3D object
*/
VkExtent3D wlu_choose_3D_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height, uint32_t depth);

/* Acquire the swapchain image in order to set its layout */
VkResult wlu_acquire_next_sc_img(vkcomp *app, uint32_t cur_scd, uint32_t *cur_img);

VkResult wlu_queue_graphics_queue(
  vkcomp *app,
  uint32_t commandBufferCount,
  VkCommandBuffer *pCommandBuffers,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  const VkPipelineStageFlags *pWaitDstStageMask,
  uint32_t signalSemaphoreCount,
  const VkSemaphore *pSignalSemaphores
);

/* Submit results back to the swap chain */
VkResult wlu_queue_present_queue(
  vkcomp *app,
  uint32_t waitSemaphoreCount,
  const VkSemaphore *pWaitSemaphores,
  uint32_t swapchainCount,
  const VkSwapchainKHR *Swapchains,
  const uint32_t *pImageIndices,
  VkResult *pResults
);

#endif
