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

#ifndef DLU_VKCOMP_TYPES_H
#define DLU_VKCOMP_TYPES_H

/* Leave for right now. This will need to be changed */
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_DISPLAY_KHR
#include <vulkan/vulkan.h>

/**
* The amount of time, one waits for a command buffer to complete
* unit measured in nanoseconds
*/
#define GENERAL_TIMEOUT 100000000

typedef enum _dlu_sync_type {
  DLU_VK_WAIT_RENDER_FENCE = 0x0000,     /* Set render fence to signal state */
  DLU_VK_WAIT_IMAGE_FENCE = 0x0001,        /* Set image fence to signal state */
  DLU_VK_WAIT_GRAPHICS_QUEUE = 0x0002, /* Synchronously wait on the present queue */
  DLU_VK_RESET_RENDER_FENCE = 0x0003,  /* Set Render Fence in unsignaled state */
  DLU_VK_GET_RENDER_FENCE = 0x0004      /* Get the state of the fence */
} dlu_sync_type;

typedef enum _dlu_destroy_type {
  DLU_DESTROY_VK_SHADER = 0x0000, /* Destroy VkShaderModule Objects */
  DLU_DESTROY_VK_BUFFER = 0x0001, /* Destroy VkBuffer Objects */
  DLU_DESTROY_VK_MEMORY = 0x0002, /* Destroy VkMemory Objects */
  DLU_DESTROY_VK_CMD_POOL = 0x0003, /* Destroy VkCommandPool Objects */
  DLU_DESTROY_VK_DESC_POOL = 0x0004, /* Destroy VkDescriptorPool Objects */
  DLU_DESTROY_VK_DESC_SET_LAYOUT = 0x0005, /* Destroy VkDescriptorSetLayout Objects */
  DLU_DESTROY_PIPELINE_CACHE = 0x0006, /* Destroy VkPipelineCache Objects */
  DLU_DESTROY_VK_FRAME_BUFFER = 0x0007, /* Destroy VkFrameBuffer Objects */
  DLU_DESTROY_VK_RENDER_PASS = 0x0008, /* Destroy VkRenderPass Objects */
  DLU_DESTROY_VK_PIPE_LAYOUT = 0x0009, /* Destroy VkPipelineLayout Objects */
  DLU_DESTROY_PIPELINE = 0x000A, /* Destroy VkPipeline Objects */
  DLU_DESTROY_VK_SAMPLER = 0x000B, /* Destroy VkSampler Objects */
  DLU_DESTROY_VK_IMAGE = 0x000C, /* Destroy VkImage Objects */
  DLU_DESTROY_VK_IMAGE_VIEW = 0x000D, /* Destroy VkImageView Objects */
  DLU_DESTROY_VK_SWAPCHAIN = 0x000E, /* Destroy VkSwapchainKHR Objects */
  DLU_DESTROY_VK_SEMAPHORE = 0x000F, /* Destroy VkSemaphore Objects */
  DLU_DESTROY_VK_FENCE = 0x0010, /* Destroy VkFence Objects */
  DLU_DESTROY_VK_LOGIC_DEVICE = 0x0011 /* Destroy VkDevice Objects */
} dlu_destroy_type;

typedef enum _dlu_image_view_type {
  DLU_SC_IMAGE_VIEWS    = 0x0000,    /* Swapchain image views */
  DLU_TEXT_IMAGE_VIEWS  = 0x0001,  /* Texture image views */
  DLU_DEPTH_IMAGE_VIEWS = 0x0002 /* Depth Buffer image view */
} dlu_image_view_type;

typedef enum _dlu_mem_map_type {
  DLU_VK_BUFFER = 0x0000,
  DLU_TEXT_VK_IMAGE = 0x0001
} dlu_mem_map_type;

typedef struct _vkcomp {
  /* Function pointers bellow are used for debugging purposes */ 
  PFN_vkQueueBeginDebugUtilsLabelEXT dbg_utils_queue_begin;
  PFN_vkQueueEndDebugUtilsLabelEXT dbg_utils_queue_end;
  PFN_vkQueueInsertDebugUtilsLabelEXT dbg_utils_queue_insert;
  PFN_vkCmdBeginDebugUtilsLabelEXT dbg_utils_cmd_begin;
  PFN_vkCmdEndDebugUtilsLabelEXT dbg_utils_cmd_end;
  PFN_vkCmdInsertDebugUtilsLabelEXT dbg_utils_cmd_insert;

  PFN_vkDestroyDebugUtilsMessengerEXT dbg_destroy_utils_msg;
  VkDebugUtilsMessengerEXT debug_utils_msg;

  struct _set_obj {
    uint32_t ldi;
    PFN_vkSetDebugUtilsObjectNameEXT dbg_utils_set_object_name;
  } set_obj;

  VkInstance instance;
  VkSurfaceKHR surface;

  uint32_t pdc; /* physical device count */
  struct _pd_data { /* physical device data */
    VkPhysicalDevice phys_dev;
    uint32_t gfam_idx; /* Graphics Queue, Queue Family Index */
    uint32_t tfam_idx; /* Transfer Queue, Queue Family Index */ 
    uint32_t cfam_idx; /* Compute Queue, Queue Family Index */
  } *pd_data;

  uint32_t ldc; /* Logical device count */
  struct _ld_data {  /* logical device data */
    VkQueue graphics;
    VkQueue transfer;
    VkQueue compute;
    VkDevice device;
    uint32_t pdi; /* Physical device data index */
  } *ld_data;

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

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *sc_data;

  struct _gp_cache {
    VkPipelineCache pipe_cache;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } gp_cache;

  uint32_t gdc;
  struct _gp_data {
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    uint32_t gpc; /* graphics piplines count */
    VkPipeline *graphics_pipelines;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *gp_data;

  uint32_t cdc; /* command data count */
  struct _cmd_data {
    VkCommandPool cmd_pool;
    VkCommandBuffer *cmd_buffs;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *cmd_data;

  uint32_t bdc; /* buffer data count */
  struct _buff_data {
    VkBuffer buff;
    VkDeviceMemory mem;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *buff_data;

  uint32_t ddc; /* descriptor data count */
  struct _desc_data {
    VkDescriptorPool desc_pool;
    uint32_t dlsc; /* descriptor layout/set count */
    VkDescriptorSetLayout *layouts;
    VkDescriptorSet *desc_set;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *desc_data;
  
  uint32_t tdc; /* texture data count */
  struct _text_data {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
    VkSampler sampler;

    /* logical device index, Used to keep track of active VkDevice */
    uint32_t ldi;
  } *text_data;
} vkcomp;

#endif
