#ifndef LUCOM_H
#define LUCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <cglm/call.h>

#define VK_USE_PLATFORM_WAYLAND_KHR 1
#include <vulkan/vulkan.h>

struct queue_family_indices {
  int graphics_family;
  int present_family;
};

struct vkcomp {
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
  VkPhysicalDeviceMemoryProperties memory_properties;
  VkPhysicalDevice physical_device;
  VkPhysicalDevice *devices;
  uint32_t device_count;

  VkDeviceQueueCreateInfo *queue_create_infos;
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

/*
 * http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/
 * https://stackoverflow.com/questions/23235910/variadic-unused-function-macro
 */
#define UNUSED1(z) (void)(z)
#define UNUSED2(y,z) UNUSED1(y),UNUSED1(z)
#define UNUSED3(x,y,z) UNUSED1(x),UNUSED2(y,z)
#define UNUSED4(b,x,y,z) UNUSED2(b,x),UNUSED2(y,z)
#define UNUSED5(a,b,x,y,z) UNUSED2(a,b),UNUSED3(x,y,z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif
