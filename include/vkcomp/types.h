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

#ifndef WLU_VKCOMP_TYPES_H
#define WLU_VKCOMP_TYPES_H

#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

/**
* The amount of time, one waits for a command buffer to complete
* unit measured in nanoseconds
*/
#define GENERAL_TIMEOUT 100000000
#define INDEX_IGNORE -1

typedef enum _wlu_sync_type {
  WLU_VK_WAIT_RENDER_FENCE   = 0x0000,  /* Set render fence to signal state */
  WLU_VK_WAIT_IMAGE_FENCE    = 0x0001,  /* Set image fence to signal state */
  WLU_VK_WAIT_PRESENT_QUEUE  = 0x0002,  /* Synchronously wait on the present queue */
  WLU_VK_WAIT_GRAPHICS_QUEUE = 0x0003,  /* Synchronously wait on the present queue */
  WLU_VK_RESET_RENDER_FENCE  = 0x0004,  /* Set Render Fence in unsignaled state */
  WLU_VK_GET_RENDER_FENCE    = 0x0005   /* Get the state of the fence */
} wlu_sync_type;

typedef enum _wlu_destroy_type {
  WLU_DESTROY_VK_SHADER           = 0x0000,  /* Destroy VkShaderModule Objects */
  WLU_DESTROY_VK_BUFFER           = 0x0001,  /* Destroy VkBuffer Objects */
  WLU_DESTROY_VK_MEMORY           = 0x0002,  /* Destroy VkMemory Objects */
  WLU_DESTROY_VK_CMD_POOL         = 0x0003,  /* Destroy VkCommandPool Objects */
  WLU_DESTROY_VK_DESC_POOL        = 0x0004,  /* Destroy VkDescriptorPool Objects */
  WLU_DESTROY_VK_DESC_SET_LAYOUT  = 0x0005,  /* Destroy VkDescriptorSetLayout Objects */
  WLU_DESTROY_PIPELINE_CACHE      = 0x0006,  /* Destroy VkPipelineCache Objects */
  WLU_DESTROY_VK_FRAME_BUFFER     = 0x0007,  /* Destroy VkFrameBuffer Objects */
  WLU_DESTROY_VK_RENDER_PASS      = 0x0008,  /* Destroy VkRenderPass Objects */
  WLU_DESTROY_VK_PIPE_LAYOUT      = 0x0009,  /* Destroy VkPipelineLayout Objects */
  WLU_DESTROY_PIPELINE            = 0x000A,  /* Destroy VkPipeline Objects */
  WLU_DESTROY_VK_SAMPLER          = 0x000B,  /* Destroy VkSampler Objects */
  WLU_DESTROY_VK_IMAGE            = 0x000C,  /* Destroy VkImage Objects */
  WLU_DESTROY_VK_IMAGE_VIEW       = 0x000D,  /* Destroy VkImageView Objects */
  WLU_DESTROY_VK_SWAPCHAIN        = 0x000E,  /* Destroy VkSwapchainKHR Objects */
  WLU_DESTROY_VK_SEMAPHORE        = 0x000F,  /* Destroy VkSemaphore Objects */
  WLU_DESTROY_VK_FENCE            = 0x0010,  /* Destroy VkFence Objects */
  WLU_DESTROY_VK_LOGIC_DEVICE     = 0x0011   /* Destroy VkDevice Objects */
} wlu_destroy_type;

typedef enum _wlu_image_view_type {
  WLU_SC_IMAGE_VIEWS   = 0x0000, /* Swapchain image views */
  WLU_TEXT_IMAGE_VIEWS = 0x0001  /* Texture image views */
} wlu_image_view_type;

typedef struct _vkcomp {
  PFN_vkDestroyDebugReportCallbackEXT dbg_destroy_report_callback;
  VkDebugReportCallbackEXT debug_report_callback;

  VkInstance instance;
  VkSurfaceKHR surface;

  VkPhysicalDevice physical_device;
  VkDevice device; /* logical device */
  struct _queue_family_indices {
    uint32_t graphics_family;
    uint32_t present_family;
  } indices;

  VkQueue graphics_queue;
  VkQueue present_queue;

  uint32_t sdc; /* swap chain data count */
  struct _sc_data {
    uint32_t sic; /* swap chain image count */
    VkSwapchainKHR swap_chain;
    struct _swap_chain_buffers {
      VkImage image;
      VkImageView view;
      VkFramebuffer fb;
    } *sc_buffs;

    /**
    * VkFence image: Used to have a sync object to wait on before a new frame can use that image
    * VkFence render: Used to signal that a frame has finished rendering
    * VkSemaphore image: Signal that a swapchaine image has been acquire
    * VkSemaphore render: Signal that an swapchain image is ready for & done rendering
    */
    struct _synchronizers {
      struct {
        VkFence image;
        VkFence render;
      } fence;

      struct {
        VkSemaphore image;
        VkSemaphore render;
      } sem;
    } *syncs;

    struct _depth_buffer {
      VkImage image;
      VkImageView view;
      VkDeviceMemory mem;
    } depth;
  } *sc_data;

  uint32_t gdc;
  VkPipelineCache pipeline_cache;
  struct _gp_data {
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    uint32_t gpc; /* graphics piplines count */
    VkPipeline *graphics_pipelines;
  } *gp_data;

  uint32_t cdc; /* command data count */
  struct _cmd_data {
    VkCommandPool cmd_pool;
    VkCommandBuffer *cmd_buffs;
  } *cmd_data;

  uint32_t bdc; /* buffer data count */
  struct _buff_data {
    VkBuffer buff;
    VkDeviceMemory mem;
    VkDeviceSize size;
    char name;
  } *buff_data;

  uint32_t ddc; /* descriptor data count */
  struct _desc_data {
    VkDescriptorPool desc_pool;
    uint32_t dlsc; /* descriptor layout/set count */
    VkDescriptorSetLayout *layouts;
    VkDescriptorSet *desc_set;
  } *desc_data;
  
  uint32_t tdc; /* texture data count */
  struct _text_data {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
    VkSampler sampler;
  } *text_data;

  uint32_t dpc; /* Display Data Count = VkDisplayProps Count */
  struct _dis_data {
    VkDisplayPropertiesKHR props;
  } *dis_data;
} vkcomp;

#endif
