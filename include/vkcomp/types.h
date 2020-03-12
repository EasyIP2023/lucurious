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

#include <cglm/types.h>

#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

/**
* The amount of time, one waits for a command buffer to complete
* unit measured in nanoseconds
*/
#define GENERAL_TIMEOUT 100000000
#define ALLOC_INDEX_IGNORE -1

typedef enum _wlu_image_view_type {
  WLU_SC_IMAGE_VIEWS = 0x0000,   /* Swapchain image views */
  WLU_TEXT_IMAGE_VIEWS = 0x0001  /* Texture image views */
} wlu_image_view_type;

typedef enum _wlu_rotate_type {
  WLU_X = 0x0000,
  WLU_Y = 0x0001,
  WLU_Z = 0x0002
} wlu_rotate_type;

typedef enum _wlu_vec_type {
  WLU_VEC2 = 0x0000,
  WLU_VEC3 = 0x0001,
  WLU_VEC4 = 0x0002
} wlu_vec_type;

typedef enum _wlu_matrix_type {
  WLU_MAT3 = 0x0000,
  WLU_MAT4 = 0x0001
} wlu_matrix_type;

typedef struct _vertex_2D {
  vec2 pos;
  vec3 color;
} vertex_2D;

typedef struct _vertex_3D {
  vec4 pos;
  vec4 color;
} vertex_3D;

typedef struct _vkcomp {
  PFN_vkDestroyDebugReportCallbackEXT dbg_destroy_report_callback;
  VkDebugReportCallbackEXT debug_report_callback;

  VkInstance instance;
  VkSurfaceKHR surface;

  VkPhysicalDevice physical_device;
  struct queue_family_indices {
    uint32_t graphics_family;
    uint32_t present_family;
  } indices;

  VkDevice device; /* logical device */
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

    struct _semaphores {
      VkSemaphore image;
      VkSemaphore render;
    } *sems;

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
    VkDescriptorSetLayout *desc_layouts;
    VkDescriptorSet *desc_set;
  } *desc_data;
  
  uint32_t tdc; /* texture data count */
  struct _text_data {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
    VkSampler sampler;
  } *text_data;
} vkcomp;

#endif
