#ifndef LUCOM_H
#define LUCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <cglm/call.h>

#include <vulkan/vulkan.h>

struct queue_family_indices {
  int graphics_family;
};

struct vkcomp {
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
  wl_display *display;
  wl_registry *registry;
  wl_compositor *compositor;
  wl_buffer *buffer;
  wl_surface *surface;
  xdg_surface *xdg_surface;
  wl_seat *seat;
  wl_shm *shm;
  xdg_wm_base *xdg_wm_base;
  xdg_toplevel *xdg_toplevel;
  void *shm_data;
  uint32_t version;
  int running;
#endif
  VkInstance instance;
  VkSurfaceKHR surface;

  /*
   * A layer can expose extensions, keep track of those
   * extensions here.
   */
  VkLayerProperties *instance_layer_properties;
  uint32_t instance_layer_count;

  /* To get device properties like the name, type and supported Vulkan version */
  VkPhysicalDeviceProperties device_properties;
  /* For optional features like texture compression,
    64 bit floats and multi viewport rendering */
  VkPhysicalDeviceFeatures device_features;
  VkPhysicalDevice physical_device;
  VkPhysicalDevice *devices;
  uint32_t device_count;

  VkQueueFamilyProperties *queue_families;
  uint32_t queue_family_count;
  struct queue_family_indices indices;

  VkDevice device; /* logical device */
  VkQueue graphics_queue;
};

/* Can find in vulkan-sdk samples/API-Samples/utils/util.hpp */
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                            \
  {                                                                        \
    info.fp##entrypoint =                                                  \
        PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);  \
    if (info.fp##entrypoint == NULL) {                                     \
      perror("vkGetDeviceProcAddr failed to find vk %s", #entrypoint);      \
      exit(-1);                                                            \
    }                                                                      \
  }

/* Can find in vulkan-sdk samples/API-Samples/utils/util.hpp */
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                  \
  {                                                                          \
    info.fp##entrypoint =                                                  \
        (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk" #entrypoint);    \
    if (info.fp##entrypoint == NULL) {                                     \
        perror("[x] vkGetDeviceProcAddr failed to find vk %s", #entrypoint);  \
        exit(-1);                                                          \
    }                                                                      \
  }

/* Can find in vulkan-sdk samples/API-Samples/utils/util.hpp */
#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#endif
