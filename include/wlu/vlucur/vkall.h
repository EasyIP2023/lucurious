#ifndef VKALL_H
#define VKALL_H

#include <cglm/call.h>

#define VK_USE_PLATFORM_WAYLAND_KHR 1
#include <vulkan/vulkan.h>

typedef struct swap_chain_buffers {
  VkImage image;
  VkImageView view;
} swap_chain_buffers;

typedef struct queue_family_indices {
  uint32_t graphics_family;
  uint32_t present_family;
} queue_family_indices;

typedef struct vkcomp {
  VkInstance instance;
  VkSurfaceKHR surface;

  /* keep track of all vulkan extensions */
  VkLayerProperties *vk_layer_props;
  uint32_t vk_layer_count;

  VkExtensionProperties *ep_instance_props;
  uint32_t ep_instance_count;

  VkExtensionProperties *ep_device_props;
  uint32_t ep_device_count;

  /* To get device properties like the name, type and supported Vulkan version */
  VkPhysicalDeviceProperties device_properties;
  /* For optional features like texture compression,
    64 bit floats and multi viewport rendering */
  VkPhysicalDeviceFeatures device_features;
  VkPhysicalDeviceMemoryProperties memory_properties;
  VkPhysicalDevice physical_device;

  VkDeviceQueueCreateInfo *queue_create_infos;
  VkQueueFamilyProperties *queue_families;
  uint32_t queue_family_count;
  queue_family_indices indices;

  VkDevice device; /* logical device */
  VkQueue graphics_queue;

  swap_chain_buffers *sc_buffs;
  VkSwapchainKHR swap_chain;
  uint32_t sc_buff_size;

  VkRenderPass render_pass;
  VkPipelineLayout pipeline_layout;
  VkPipeline graphics_pipeline;

  VkFramebuffer *sc_frame_buffs;

  VkCommandPool cmd_pool;
  VkCommandBuffer *cmd_buffs;
} vkcomp;

/* Function protypes */
vkcomp *wlu_init_vk();

VkResult wlu_set_global_layers(vkcomp *app);

VkResult wlu_create_instance(vkcomp *app, char *app_name, char *engine_name);

VkResult wlu_enumerate_devices(vkcomp *app, VkQueueFlagBits vkqfbits, VkPhysicalDeviceType vkpdtype);

VkResult wlu_create_logical_device(vkcomp *app);

VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface);

VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app);

VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace);

VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app);

VkExtent2D wlu_choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height);

VkResult wlu_create_swap_chain(
  vkcomp *app,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR pres_mode,
  VkExtent2D extent
);

VkResult wlu_create_img_views(vkcomp *app, VkFormat format, VkImageViewType type);

VkResult wlu_create_framebuffers(vkcomp *app, uint32_t attachment_count, VkExtent2D extent, uint32_t layers);

VkResult wlu_create_cmd_pool(vkcomp *app, VkCommandPoolCreateFlagBits flags);

VkResult wlu_create_cmd_buffs(vkcomp *app, VkCommandBufferLevel level);

VkResult wlu_start_cmd_buff_record(
  vkcomp *app,
  VkCommandBufferUsageFlags flags,
  const VkCommandBufferInheritanceInfo *pInheritanceInfo
);

VkResult wlu_stop_cmd_buff_record(vkcomp *app);

void wlu_freeup_vk(void *data);

#endif
