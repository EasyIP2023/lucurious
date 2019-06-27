#ifndef VKALL_H
#define VKALL_H

#include <cglm/call.h>

#define VK_USE_PLATFORM_WAYLAND_KHR 1
#include <vulkan/vulkan.h>

enum wlu_image { one_d_img, two_d_img, three_d_img };

struct swap_chain_buffers {
  VkImage image;
  VkImageView view;
};

struct queue_family_indices {
  uint32_t graphics_family;
  uint32_t present_family;
};

struct vkcomp {
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
  struct queue_family_indices indices;

  VkDevice device; /* logical device */
  VkQueue graphics_queue;

  struct swap_chain_buffers *sc_buffs;
  VkSwapchainKHR swap_chain;
  VkFormat sc_img_fmt;
  VkExtent2D sc_extent;
  uint32_t img_count;
};

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
struct vkcomp *wlu_init_vk();
VkResult wlu_set_global_layers(struct vkcomp *app);
VkResult wlu_create_instance(struct vkcomp *app, char *app_name, char *engine_name);
VkResult wlu_enumerate_devices(struct vkcomp *app);
VkResult wlu_set_logical_device(struct vkcomp *app);
VkResult wlu_vkconnect_surfaceKHR(struct vkcomp *app, void *wl_display, void *wl_surface);
VkResult wlu_create_swap_chain(struct vkcomp *app);
VkResult wlu_create_img_views(struct vkcomp *app, enum wlu_image type);
VkResult wlu_create_graphics_pipeline(struct vkcomp *app);
void wlu_freeup_vk(void *data);

#endif
