#ifndef VKALL_H
#define VKALL_H

#include <cglm/call.h>

#define VK_USE_PLATFORM_WAYLAND_KHR 1
#include <vulkan/vulkan.h>

typedef enum wlu_image_type {
  ONE_D_IMG = 1,
  TWO_D_IMG = 2,
  THREE_D_IMG = 3,
  WLU_MAX_IMAGE_ENUM = 0x7FFFFFFF
} wlu_image_type;

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
  VkFormat sc_img_fmt;
  VkExtent2D sc_extent;
  uint32_t img_count;
} vkcomp;

/* Can find in vulkan-sdk samples/API-Samples/utils/util.hpp */
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                    \
  {                                                                                \
    info.fp##entrypoint =                                                          \
        PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);          \
    if (info.fp##entrypoint == NULL) {                                             \
      fprintf(stderr, "[x] vkGetDeviceProcAddr failed to find vk %s", #entrypoint); \
      exit(-1);                                                                    \
    }                                                                              \
  }

/* Can find in vulkan-sdk samples/API-Samples/utils/util.hpp */
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                          \
  {                                                                                   \
    info.fp##entrypoint =                                                             \
        (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk" #entrypoint);               \
    if (info.fp##entrypoint == NULL) {                                                \
        fprintf(stderr, "[x] vkGetDeviceProcAddr failed to find vk %s", #entrypoint);  \
        exit(-1);                                                                     \
    }                                                                                 \
  }

/* Function protypes */
vkcomp *wlu_init_vk();
VkResult wlu_set_global_layers(vkcomp *app);
VkResult wlu_create_instance(vkcomp *app, char *app_name, char *engine_name);
VkResult wlu_enumerate_devices(vkcomp *app, VkQueueFlagBits vkqfbits, VkPhysicalDeviceType vkpdtype);
VkResult wlu_set_logical_device(vkcomp *app);
VkResult wlu_vkconnect_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface);
VkSurfaceCapabilitiesKHR wlu_q_device_capabilities(vkcomp *app);
VkSurfaceFormatKHR wlu_choose_swap_surface_format(vkcomp *app, VkFormat format, VkColorSpaceKHR colorSpace);
VkPresentModeKHR wlu_choose_swap_present_mode(vkcomp *app);
VkExtent2D wlu_choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities);
VkResult wlu_create_swap_chain(vkcomp *app, VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt, VkPresentModeKHR pres_mode, VkExtent2D extent);
VkResult wlu_create_img_views(vkcomp *app, wlu_image_type type);
VkResult wlu_create_gp(vkcomp *app, uint32_t num_args, ...);
void wlu_freeup_vk(void *data);

#endif
