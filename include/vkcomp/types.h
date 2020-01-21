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

#ifndef TYPES_H
#define TYPES_H

/**
* The amount of time, one waits for a command buffer to complete
* unit measured in nanoseconds
*/
#define GENERAL_TIMEOUT 100000000
#define ALLOC_INDEX_NON 0

typedef struct _vertex_2D {
  vec2 pos;
  vec3 color;
} vertex_2D;

typedef struct _vertex_3D {
  vec4 pos;
  vec4 color;
} vertex_3D;

typedef enum _wlu_rotate_type {
  WLU_X = 0x00000000,
  WLU_Y = 0x00000001,
  WLU_Z = 0x00000002
} wlu_rotate_type;

typedef enum _wlu_vec_type {
  WLU_VEC2 = 0x00000000,
  WLU_VEC3 = 0x00000001,
  WLU_VEC4 = 0x00000002
} wlu_vec_type;

typedef enum _wlu_matrix_type {
  WLU_MAT3 = 0x00000000,
  WLU_MAT4 = 0x00000001
} wlu_matrix_type;

typedef enum _wlu_data_type {
  WLU_SC_DATA = 0x00000000,
  WLU_GP_DATA = 0x00000001,
  WLU_CMD_DATA = 0x00000002,
  WLU_BUFFS_DATA = 0x00000003,
  WLU_DESC_DATA = 0x00000004,
  WLU_SC_DATA_MEMS = 0x00000005,
  WLU_DESC_DATA_MEMS = 0x00000006,
  WLU_GP_DATA_MEMS = 0x00000007
} wlu_data_type;

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
    struct swap_chain_buffers {
      VkImage image;
      VkImageView view;
      VkFramebuffer fb;
    } *sc_buffs;

    struct semaphores {
      VkSemaphore image;
      VkSemaphore render;
    } *sems;

    struct depth_buffer {
      VkFormat format;
      VkImage image;
      VkDeviceMemory mem;
      VkImageView view;
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
  struct _buffs_data {
    VkBuffer buff;
    VkDeviceMemory mem;
    VkDeviceSize size;
    char name;
  } *buffs_data;

  uint32_t ddc; /* descriptor data count */
  struct _desc_data {
    VkDescriptorPool desc_pool;
    uint32_t dlsc; /* descriptor layout/set count */
    VkDescriptorSetLayout *desc_layouts;
    VkDescriptorSet *desc_set;
  } *desc_data;
} vkcomp;

#endif
