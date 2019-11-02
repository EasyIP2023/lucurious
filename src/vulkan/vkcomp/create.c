/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 EasyIP2023
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

#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
#include <vlucur/utils.h>
#include <vlucur/device.h>
#include <vlucur/values.h>

VkResult wlu_create_instance(
  vkcomp *app,
  char *app_name,
  char *engine_name,
  uint32_t enabledLayerCount,
  const char *const *ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  /* initialize the VkApplicationInfo structure */
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = engine_name;
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /* tells the Vulkan driver which instance extensions
    and global validation layers we want to use */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = enabledLayerCount;
  create_info.ppEnabledLayerNames = ppEnabledLayerNames;
  create_info.enabledExtensionCount = enabledExtensionCount;
  create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;

  /* Create the instance */
  res = vkCreateInstance(&create_info, NULL, &app->instance);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateInstance failed, ERROR CODE: %d", res);
    return res;
  }

  res = get_extension_properties(app, NULL, NULL);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] get_extension_properties failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}

/* Get user physical device */
VkResult wlu_create_physical_device(
  vkcomp *app,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkPhysicalDevice *devices = VK_NULL_HANDLE;
  uint32_t device_count = 0;

  if (!app->instance) {
    wlu_log_me(WLU_DANGER, "[x] A VkInstance must be established");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_instance(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_devices;
  }

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, NULL);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkEnumeratePhysicalDevices failed, ERROR CODE: %d", res);
    goto finish_devices;
  }

  if (device_count == 0) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] failed to find GPUs with Vulkan support!!! device_count equals 0");
    goto finish_devices;
  }

  devices = (VkPhysicalDevice *) calloc(sizeof(VkPhysicalDevice),
      device_count * sizeof(VkPhysicalDevice));
  if (!devices) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] calloc VkPhysicalDevice *devices failed");
    goto finish_devices;
  }

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, devices);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkEnumeratePhysicalDevices failed, ERROR CODE: %d", res);
    goto finish_devices;
  }

  /*
  * get a physical device that is suitable
  * to do the graphics related task that we need
  */
  for (uint32_t i = 0; i < device_count; i++) {
    if (is_device_suitable(devices[i], vkpdtype, device_props, device_feats) &&
        /* Check if current device has swap chain support */
        get_extension_properties(app, NULL, devices[i])) {
      memcpy(&app->physical_device, &devices[i], sizeof(devices[i]));
      wlu_log_me(WLU_SUCCESS, "Suitable GPU Found: %s", device_props->deviceName);
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] failed to find a suitable GPU!!!");
    goto finish_devices;
  }

finish_devices:
  if (devices) {
    free(devices);
    devices = NULL;
  }
  return res;
}

VkResult wlu_create_logical_device(
  vkcomp *app,
  VkPhysicalDeviceFeatures *device_feats,
  uint32_t enabledLayerCount,
  const char *const *ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
) {

  VkResult res = VK_RESULT_MAX_ENUM;
  float queue_priorities[1] = {1.0};

  if (!app->physical_device) {
    wlu_log_me(WLU_DANGER, "[x] A physical device must be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_physical_device(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  if (!app->queue_families) {
    wlu_log_me(WLU_DANGER, "[x] At least one queue family should be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_queue_family(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->queue_create_infos = (VkDeviceQueueCreateInfo *) calloc(sizeof(VkDeviceQueueCreateInfo),
      app->queue_family_count * sizeof(VkDeviceQueueCreateInfo));
  if (!app->queue_create_infos) {
    wlu_log_me(WLU_DANGER, "[x] calloc app->queue_create_infos failed");
    return res;
  }

  uint32_t queue_fam_indices[] = {app->indices.graphics_family, app->indices.present_family};
  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    app->queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    app->queue_create_infos[i].pNext = NULL;
    app->queue_create_infos[i].flags = 0;
    app->queue_create_infos[i].queueFamilyIndex = queue_fam_indices[i];
    app->queue_create_infos[i].queueCount = app->queue_family_count;
    app->queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.queueCreateInfoCount = app->queue_family_count;
  create_info.pQueueCreateInfos = app->queue_create_infos;
  create_info.enabledLayerCount = enabledLayerCount;
  create_info.ppEnabledLayerNames = ppEnabledLayerNames;
  create_info.enabledExtensionCount = enabledExtensionCount;
  create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;
  create_info.pEnabledFeatures = device_feats;


  /* Create logic device */
  res = vkCreateDevice(app->physical_device, &create_info, NULL, &app->device);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateDevice failed, ERROR CODE: %d", res);
    return res;
  }

  /*
   * Queues are automatically created with
   * the logical device, but you need a queue
   * handle to interface with them
   */
  vkGetDeviceQueue(app->device, app->indices.graphics_family, 0, &app->graphics_queue);
  if (app->indices.graphics_family == app->indices.present_family)
    app->present_queue = app->graphics_queue;
  else
    vkGetDeviceQueue(app->device, app->indices.present_family, 0, &app->present_queue);

  return res;
}

VkResult wlu_create_swap_chain(
  vkcomp *app,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR pres_mode,
  uint32_t width,
  uint32_t height
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->surface) {
    wlu_log_me(WLU_DANGER, "[x] app->surface must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_vkconnect_surfaceKHR(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  if (!app->device) {
    wlu_log_me(WLU_DANGER, "[x] app->device must be initialize");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_logical_device(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->sc = (struct swap_chain *) realloc(app->sc, (app->scc+1) * sizeof(struct swap_chain));
  if (!app->sc) {
    wlu_log_me(WLU_DANGER, "realloc app->sc failed!");
    return res;
  }

  /* initialize swap chain values */
  set_sc_init_values(app);

  /*
   * Don't want to stick to minimum becuase one would have to wait on the
   * drive to complete internal operations before one can acquire another
   * images to render to. So it's recommended to add one to minImageCount
   */
  app->sc[app->scc].sic = capabilities.minImageCount + 1;

  /* Be sure sic doesn't exceed the maximum. */
  if (capabilities.maxImageCount > 0 && app->sc[app->scc].sic > capabilities.maxImageCount)
    app->sc[app->scc].sic = capabilities.maxImageCount;

  VkSurfaceTransformFlagBitsKHR pre_transform;
  pre_transform = (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? \
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : capabilities.currentTransform;

  VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  VkCompositeAlphaFlagBitsKHR composite_alpha_flags[4] = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };

  for (uint32_t i = 0; i < sizeof(composite_alpha_flags); i++) {
    if (capabilities.supportedCompositeAlpha & composite_alpha_flags[i]) {
      composite_alpha = composite_alpha_flags[i];
      break;
    }
  }

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.surface = app->surface;
  create_info.minImageCount = app->sc[app->scc].sic;
  create_info.imageFormat = surface_fmt.format;
  create_info.imageColorSpace = surface_fmt.colorSpace;
  create_info.imageExtent.width = width;
  create_info.imageExtent.height = height;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  /* current transform should be applied to images in the swap chain */
  create_info.preTransform = pre_transform;
  /* specify that I currently do not want any transformation */
  create_info.compositeAlpha = composite_alpha;
  create_info.presentMode = pres_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  /* specify how to handle swap chain images that will be used across multiple queue families */
  if (app->indices.graphics_family != app->indices.present_family) {
    const uint32_t queue_family_indices[2] = {
      app->indices.graphics_family,
      app->indices.present_family
    };
    /* images can be used across multiple queue families */
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    /* image is owned by one queue family at a time, Best for performance */
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;
  }

  res = vkCreateSwapchainKHR(app->device, &create_info, NULL, &app->sc[app->scc].swap_chain);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateSwapchainKHR failed, ERROR CODE: %d", res);
    return res;
  }

  app->scc++;

  return res;
}

VkResult wlu_create_img_views(
  vkcomp *app,
  uint32_t cur_sc,
  VkFormat format,
  VkImageViewType type
) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkImage *sc_imgs = NULL;

  if (!app->sc[cur_sc].swap_chain) {
    wlu_log_me(WLU_DANGER, "[x] Swap Chain doesn't exists");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_swap_chain(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_create_img_views;
  }

  app->sc[cur_sc].sc_buffs = (struct swap_chain_buffers *) calloc(sizeof(struct swap_chain_buffers),
        app->sc[cur_sc].sic * sizeof(struct swap_chain_buffers));
  if (!app->sc[cur_sc].sc_buffs) {
    wlu_log_me(WLU_DANGER, "[x] calloc app->sc[%d].sc_buffs failed", cur_sc);
    goto finish_create_img_views;
  }

  set_sc_buffs_init_values(app, cur_sc);

  /*
   * It's okay to reuse app->sc[cur_sc].sic,
   * It'll give same result as minImageCount + 1.
   * Removal of function will result in validation layer errors
   */
  res = vkGetSwapchainImagesKHR(app->device, app->sc[cur_sc].swap_chain, &app->sc[cur_sc].sic, NULL);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkGetSwapchainImagesKHR failed, ERROR CODE: %d", res);
    goto finish_create_img_views;
  }

  sc_imgs = (VkImage *) calloc(sizeof(VkImage), app->sc[cur_sc].sic * sizeof(VkImage));
  if (!sc_imgs) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] calloc VkImage *sc_imgs failed");
    goto finish_create_img_views;
  }

  res = vkGetSwapchainImagesKHR(app->device, app->sc[cur_sc].swap_chain, &app->sc[cur_sc].sic, sc_imgs);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkGetSwapchainImagesKHR failed, ERROR CODE: %d", res);
    goto finish_create_img_views;
  }

  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = app->sc[cur_sc].sc_buffs[i].image = sc_imgs[i];
    create_info.viewType = type;
    create_info.format = format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_R;
    create_info.components.g = VK_COMPONENT_SWIZZLE_G;
    create_info.components.b = VK_COMPONENT_SWIZZLE_B;
    create_info.components.a = VK_COMPONENT_SWIZZLE_A;
    /* describe what the image’s purpose is and which
        part of the image should be accessed */
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    res = vkCreateImageView(app->device, &create_info, NULL, &app->sc[cur_sc].sc_buffs[i].view);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateImageView failed, ERROR CODE: %d", res);
      goto finish_create_img_views;
    }
  }

finish_create_img_views:
  if (sc_imgs) {
    free(sc_imgs);
    sc_imgs = NULL;
  }
  return res;
}

VkResult wlu_create_depth_buff(
  vkcomp *app,
  uint32_t cur_sc,
  VkFormat depth_format,
  VkFormatFeatureFlags linearTilingFeatures,
  VkFormatFeatureFlags optimalTilingFeatures,
  VkImageType imageType,
  VkExtent3D extent,
  VkImageUsageFlags usage,
  VkSharingMode sharingMode,
  VkImageLayout initialLayout,
  VkImageViewType viewType
) {

  VkResult res = VK_RESULT_MAX_ENUM;
  VkBool32 pass;

  app->sc[cur_sc].depth.format = depth_format;

  VkImageCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.imageType = imageType;
  create_info.format = app->sc[cur_sc].depth.format;
  create_info.extent.width = extent.width;
  create_info.extent.height = extent.height;
  create_info.extent.depth = extent.depth;
  create_info.mipLevels = 1;
  create_info.arrayLayers = 1;
  create_info.samples = VK_SAMPLE_COUNT_1_BIT;

  VkFormatProperties props;
  vkGetPhysicalDeviceFormatProperties(app->physical_device, app->sc[cur_sc].depth.format, &props);
  if (props.linearTilingFeatures & linearTilingFeatures) {
    create_info.tiling = VK_IMAGE_TILING_LINEAR;
  } else if (props.optimalTilingFeatures & optimalTilingFeatures) {
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  } else {
    wlu_log_me(WLU_DANGER, "[x] Depth format currently not supported.\n");
    return res;
  }

  create_info.usage = usage;
  create_info.sharingMode = sharingMode;
  /* Come back to me */
  create_info.queueFamilyIndexCount = 0;
  create_info.pQueueFamilyIndices = NULL;
  /* Come back to me */
  create_info.initialLayout = initialLayout;

  VkMemoryAllocateInfo mem_alloc = {};
  mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_alloc.pNext = NULL;
  mem_alloc.allocationSize = 0;
  mem_alloc.memoryTypeIndex = 0;

  VkImageViewCreateInfo create_view_info = {};
  create_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_view_info.pNext = NULL;
  create_view_info.flags = 0;
  create_view_info.image = VK_NULL_HANDLE;
  create_view_info.format = app->sc[cur_sc].depth.format;
  create_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
  create_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
  create_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
  create_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
  create_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  create_view_info.subresourceRange.baseMipLevel = 0;
  create_view_info.subresourceRange.levelCount = 1;
  create_view_info.subresourceRange.baseArrayLayer = 0;
  create_view_info.subresourceRange.layerCount = 1;
  create_view_info.viewType = viewType;

  if (app->sc[cur_sc].depth.format == VK_FORMAT_D16_UNORM_S8_UINT ||
      app->sc[cur_sc].depth.format == VK_FORMAT_D24_UNORM_S8_UINT ||
      app->sc[cur_sc].depth.format == VK_FORMAT_D32_SFLOAT_S8_UINT)
      create_view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

  /* Create image object */
  res = vkCreateImage(app->device, &create_info, NULL, &app->sc[cur_sc].depth.image);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateImage failed, ERROR CODE: %d", res);
    return res;
  }

  /*
   * Although you know the width, height, and the size of a buffer element,
   * there is no way to determine exactly how much memory is needed to allocate.
   * This is because alignment constraints that may be placed by the GPU hardware.
   * This function allows you to find out everything you need to allocate the
   * memory for an image.
   */
  VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(app->device, app->sc[cur_sc].depth.image, &mem_reqs);

  mem_alloc.allocationSize = mem_reqs.size;
  /* Use the memory properties to determine the type of memory required */
  pass = memory_type_from_properties(app, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
  if (!pass) {
    wlu_log_me(WLU_DANGER, "[x] memory_type_from_properties failed");
    return pass;
  }

  /* Allocate memory */
  res = vkAllocateMemory(app->device, &mem_alloc, NULL, &app->sc[cur_sc].depth.mem);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateMemory failed, ERROR CODE: %d", res);
    return res;
  }

  /* Associate memory with image object by binding */
  res = vkBindImageMemory(app->device, app->sc[cur_sc].depth.image, app->sc[cur_sc].depth.mem, 0);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkBindImageMemory failed, ERROR CODE: %d", res);
    return res;
  }

  /* Create image view object */
  create_view_info.image = app->sc[cur_sc].depth.image;
  res = vkCreateImageView(app->device, &create_view_info, NULL, &app->sc[cur_sc].depth.view);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateImageView failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}

VkResult wlu_create_buffer(
  vkcomp *app,
  VkDeviceSize size,
  void *data,
  VkBufferCreateFlagBits flags,
  VkBufferUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  char *buff_name,
  VkFlags requirements_mask
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = sharingMode;
  create_info.queueFamilyIndexCount = queueFamilyIndexCount;
  create_info.pQueueFamilyIndices = pQueueFamilyIndices;

  app->buffs_data = (struct buffs_data *) realloc(app->buffs_data, (app->bdc+1) * sizeof(struct buffs_data));
  if (!app->buffs_data) {
    wlu_log_me(WLU_DANGER, "[x] realloc struct buffs_data *buffs_data failed!");
    return res;
  }

  set_buffs_init_values(app);

  app->buffs_data[app->bdc].name = buff_name;
  res = vkCreateBuffer(app->device, &create_info, NULL, &app->buffs_data[app->bdc].buff);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateBuffer failed, ERROR CODE: %d", res);
    return res;
  }

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(app->device, app->buffs_data[app->bdc].buff, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.allocationSize = mem_reqs.size;
  alloc_info.memoryTypeIndex = 0;

  res = memory_type_from_properties(app, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (!res) {
    wlu_log_me(WLU_DANGER, "[x] memory_type_from_properties failed");
    return res;
  }

  res = vkAllocateMemory(app->device, &alloc_info, NULL, &app->buffs_data[app->bdc].mem);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateMemory failed, ERROR CODE: %d", res);
    return res;
  }

  /* associate the memory allocated with the buffer object */
  res = vkBindBufferMemory(app->device, app->buffs_data[app->bdc].buff, app->buffs_data[app->bdc].mem, 0);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkBindBufferMemory failed, ERROR CODE: %d", res);
    return res;
  }

  /*
   * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
   * With any buffer, you need to populate it with the data that
   * you want the shader to read. In order to get CPU access to
   * the memory, you need to map it
   */
  void *p_data = NULL;
  res = vkMapMemory(app->device, app->buffs_data[app->bdc].mem, 0, mem_reqs.size, 0, &p_data);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkMapMemory failed, ERROR CODE: %d", res);
    return res;
  }

  if (data) {
    p_data = memmove(p_data, data, size);
    if (!p_data) {
      wlu_log_me(WLU_DANGER, "[x] void *p_data memmove failed");
      return res;
    }
  }

  VkMappedMemoryRange flush_range;
  flush_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  flush_range.pNext = NULL;
  flush_range.memory = app->buffs_data[app->bdc].mem;
  /* the region that was modified will be flushed */
  app->buffs_data[app->bdc].buff_info.offset = flush_range.offset = 0;
  app->buffs_data[app->bdc].buff_info.range = flush_range.size = size;
  /* from offset 0 to size of buffer */

  /* refresh the cache */
  res = vkFlushMappedMemoryRanges(app->device, 1, &flush_range);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkFlushMappedMemoryRanges failed, ERROR CODE: %d", res);
    return res;
  }

  vkUnmapMemory(app->device, app->buffs_data[app->bdc].mem);

  app->buffs_data[app->bdc].buff_info.buffer = app->buffs_data[app->bdc].buff;
  app->bdc++;

  return res;
}

/*
 * This function creates the framebuffers.
 * Attachments specified when creating the render pass
 * are bounded by wrapping them into a VkFramebuffer object.
 * A framebuffer object references all VkImageView objects this
 * is represented by "attachments"
 */
VkResult wlu_create_framebuffers(
  vkcomp *app,
  uint32_t cur_sc,
  uint32_t attachmentCount,
  VkImageView *attachments,
  uint32_t width,
  uint32_t height,
  uint32_t layers
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->render_pass) {
    wlu_log_me(WLU_DANGER, "[x] render pass not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_render_pass(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  if (!app->sc[cur_sc].sc_buffs) {
    wlu_log_me(WLU_DANGER, "[x] Swap Chain buffers not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_img_views(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->sc[cur_sc].frame_buffs = (VkFramebuffer *) calloc(sizeof(VkFramebuffer),
        app->sc[cur_sc].sic * sizeof(VkFramebuffer));
  if (!app->sc[cur_sc].frame_buffs) {
    wlu_log_me(WLU_DANGER, "[x] calloc app->sc[%d].frame_buffs failed", cur_sc);
    return res;
  }

  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    attachments[0] = app->sc[cur_sc].sc_buffs[i].view;

    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = app->render_pass;
    create_info.attachmentCount = attachmentCount;
    create_info.pAttachments = attachments;
    create_info.width = width;
    create_info.height = height;
    create_info.layers = layers;

    res = vkCreateFramebuffer(app->device, &create_info, NULL, &app->sc[cur_sc].frame_buffs[i]);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateFramebuffer failed, ERROR CODE: %d", res);
      return res;
    }
  }

  return res;
}

VkResult wlu_create_cmd_pool(vkcomp *app, VkCommandPoolCreateFlagBits flags) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->indices.graphics_family == UINT32_MAX || app->indices.present_family == UINT32_MAX) {
    wlu_log_me(WLU_DANGER, "[x] graphics or present family index not set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_queue_family(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->cmd_pbs = (struct vkcmds *) realloc(app->cmd_pbs, (app->cpc+1) * sizeof(struct vkcmds));
  if (!app->cmd_pbs) {
    wlu_log_me(WLU_DANGER, "[x] realloc struct vkcmds *cmd_pbs failed!");
    return res;
  }

  set_cmd_pbs_init_values(app);

  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueFamilyIndex = app->indices.graphics_family;

  res = vkCreateCommandPool(app->device, &create_info, NULL, &app->cmd_pbs[app->cpc].cmd_pool);
  app->cpc++;

  return res;
}

VkResult wlu_create_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_sc,
  VkCommandBufferLevel level
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->sc[cur_sc].sic == 0) {
    wlu_log_me(WLU_DANGER, "[x] Swapchain image count not set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_swap_chain(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  if (!app->cmd_pbs[cur_pool].cmd_pool) {
    wlu_log_me(WLU_DANGER, "[x] Must have a valid command pool");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_cmd_pool(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  /*
   * make it so if swap chain is destroy you don't
   * call calloc multiple time on memory already allocated
   */
  if (!app->cmd_pbs[cur_pool].cmd_buffs) { /* decided on this instead of realloc */
    app->cmd_pbs[cur_pool].cmd_buffs = (VkCommandBuffer *) calloc(sizeof(VkCommandBuffer),
          app->sc[cur_sc].sic * sizeof(VkCommandBuffer));
    if (!app->cmd_pbs[cur_pool].cmd_buffs) {
      wlu_log_me(WLU_DANGER, "[x] calloc VkCommandBuffer *cmd_buffs failed");
      return res;
    }
  }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.commandPool = app->cmd_pbs[cur_pool].cmd_pool;
  alloc_info.level = level;
  alloc_info.commandBufferCount = (uint32_t) app->sc[cur_sc].sic;

  res = vkAllocateCommandBuffers(app->device, &alloc_info, app->cmd_pbs[cur_pool].cmd_buffs);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkAllocateCommandBuffers failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}

/*
 * (This comment is for me)
 * Use a image semaphore to signal that an image
 * has been acquired and is ready for rendering.
 * Use a render semaphore to singal that rendering
 * has finished and presentation can happen.
 */
VkResult wlu_create_semaphores(vkcomp *app, uint32_t cur_sc) {
  VkResult res = VK_RESULT_MAX_ENUM;

  app->sc[cur_sc].sems = (struct semaphores *) calloc(sizeof(struct semaphores),
      app->sc[cur_sc].sic * sizeof(struct semaphores));
  if (!app->sc[cur_sc].sems) {
    wlu_log_me(WLU_DANGER, "[x] calloc semaphores *sems failed");
    return res;
  }

  set_sc_sems_init_values(app, cur_sc);

  VkSemaphoreCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;

  for (uint32_t i = 0; i < app->sc[cur_sc].sic; i++) {
    res = vkCreateSemaphore(app->device, &create_info, NULL, &app->sc[cur_sc].sems[i].image);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateSemaphore failed to create image semaphore, ERROR CODE: %d", res);
      return res;
    }

    res = vkCreateSemaphore(app->device, &create_info, NULL, &app->sc[cur_sc].sems[i].render);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateSemaphore failed to create render semaphore, ERROR CODE: %d", res);
      return res;
    }
  }

  return res;
}
