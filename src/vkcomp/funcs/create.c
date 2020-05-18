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

#define LUCUR_VKCOMP_API
#include <lucom.h>

/**
* alloca()'s usage here is meant for stack space efficiency
* Fixed size arrays tend to over allocate, while alloca will
* allocate the exact amount of bytes that you want
*/

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

  /**
  * tells the Vulkan driver which instance extensions
  * and global validation layers we want to use
  */
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
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateInstance"); }

  return res;
}

VkResult wlu_create_vkwayland_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->instance) { PERR(WLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = (struct wl_display *) wl_display;
  create_info.surface = (struct wl_surface *) wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkCreateWaylandSurfaceKHR")

  return res;
}

VkResult wlu_create_physical_device(
  vkcomp *app,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkPhysicalDevice *devices = VK_NULL_HANDLE;
  uint32_t device_count = 0;

  if (!app->instance) { PERR(WLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, NULL);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkEnumeratePhysicalDevices"); return res; }

  if (device_count == 0) {
    wlu_log_me(WLU_DANGER, "[x] failed to find GPU with Vulkan support!!!");
    return VK_RESULT_MAX_ENUM;
  }

  devices = (VkPhysicalDevice *) alloca(device_count * sizeof(VkPhysicalDevice));

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, devices);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkEnumeratePhysicalDevices"); return res; }

  /**
  * get a physical device that is suitable
  * to do the graphics related task that we need
  */
  for (uint32_t i = 0; i < device_count; i++) {
    if (is_device_suitable(devices[i], vkpdtype, device_props, device_feats)) {
      memmove(&app->physical_device, &devices[i], sizeof(devices[i]));
      wlu_log_me(WLU_SUCCESS, "Suitable GPU Found: %s", device_props->deviceName);
      break;
    }
  }

  if (app->physical_device == VK_NULL_HANDLE) {
    wlu_log_me(WLU_DANGER, "[x] failed to find a suitable GPU!!!");
    return VK_RESULT_MAX_ENUM;
  }

  return res;
}

VkBool32 wlu_create_queue_families(vkcomp *app, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 *present_support = NULL;
  VkQueueFamilyProperties *queue_families = NULL;
  uint32_t qfc = 0; /* queue family count */

  if (!app->physical_device) { PERR(WLU_VKCOMP_PHYS_DEV, 0, NULL); return ret; }

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, NULL);

  queue_families = (VkQueueFamilyProperties *) alloca(qfc * sizeof(VkQueueFamilyProperties));

  vkGetPhysicalDeviceQueueFamilyProperties(app->physical_device, &qfc, queue_families);

  present_support = (VkBool32 *) alloca(qfc * sizeof(VkBool32));

  if (app->surface)
    for (uint32_t i = 0; i < qfc; i++) /* Check for present queue family */
      vkGetPhysicalDeviceSurfaceSupportKHR(app->physical_device, i, app->surface, &present_support[i]);

  for (uint32_t i = 0; i < qfc; i++) {
    if (queue_families[i].queueFlags & vkqfbits) {
      if (app->indices.graphics_family == UINT32_MAX) {
        /* Retrieve Graphics Family Queue index */
        app->indices.graphics_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device has support for provided Queue Family");
      }

      /* Check to see if a device can present images onto a surface */
      if (app->surface && present_support[i]) {
        /* Retrieve Present Family Queue index */
        app->indices.present_family = i; ret = VK_FALSE;
        wlu_log_me(WLU_SUCCESS, "Physical Device Surface has presentation support");
        break;
      }
    }
  }

  if (app->surface && app->indices.present_family == UINT32_MAX) {
    for (uint32_t i = 0; i < qfc; i++) {
      if (present_support[i]) {
        app->indices.present_family = i; ret = VK_FALSE;
        break;
      }
    }
  }

  return ret;
}


VkResult wlu_create_logical_device(
  vkcomp *app,
  VkPhysicalDeviceFeatures *pEnabledFeatures,
  uint32_t queue_count,
  uint32_t enabledLayerCount,
  const char *const *ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
) {

  VkResult res = VK_RESULT_MAX_ENUM;
  VkDeviceQueueCreateInfo *pQueueCreateInfos = NULL;
  float queue_priorities[1] = {1.0};

  if (!app->physical_device) { PERR(WLU_VKCOMP_PHYS_DEV, 0, NULL); return res; }
  if (app->indices.graphics_family == UINT32_MAX || app->indices.present_family == UINT32_MAX) { PERR(WLU_VKCOMP_INDICES, 0, NULL); return res; }

  /* Will need to change this later but for now, These two hardware queues should currently always be the same */
  uint32_t queue_fam_indices[2] = {app->indices.graphics_family, app->indices.present_family};
  uint32_t dq_count = 1;

  pQueueCreateInfos = (VkDeviceQueueCreateInfo *) alloca(dq_count * sizeof(VkDeviceQueueCreateInfo));

  for (uint32_t i = 0; i < dq_count; i++) {
    pQueueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    pQueueCreateInfos[i].pNext = NULL;
    pQueueCreateInfos[i].flags = 0;
    pQueueCreateInfos[i].queueFamilyIndex = queue_fam_indices[i];
    pQueueCreateInfos[i].queueCount = queue_count;
    pQueueCreateInfos[i].pQueuePriorities = queue_priorities;
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.queueCreateInfoCount = dq_count;
  create_info.pQueueCreateInfos = pQueueCreateInfos;
  create_info.enabledLayerCount = enabledLayerCount;
  create_info.ppEnabledLayerNames = ppEnabledLayerNames;
  create_info.enabledExtensionCount = enabledExtensionCount;
  create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;
  create_info.pEnabledFeatures = pEnabledFeatures;

  /* Create logic device */
  res = vkCreateDevice(app->physical_device, &create_info, NULL, &app->device);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateDevice"); return res; }

  /**
  * Queues are automatically created with
  * the logical device, but you need a vkqueue
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
  uint32_t cur_scd,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR presentMode,
  uint32_t width,
  uint32_t height,
  uint32_t imageArrayLayers,
  VkImageUsageFlags imageUsage
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->surface) { PERR(WLU_VKCOMP_SURFACE, 0, NULL); return res; }
  if (!app->device) { PERR(WLU_VKCOMP_DEVICE, 0, NULL); return res; }
  if (!app->sc_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_SC_DATA"); return res; }

  VkCompositeAlphaFlagBitsKHR ca_flags[4] = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
  };

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.surface = app->surface;
  create_info.minImageCount = app->sc_data[cur_scd].sic;
  create_info.imageFormat = surface_fmt.format;
  create_info.imageColorSpace = surface_fmt.colorSpace;
  create_info.imageExtent.width = width;
  create_info.imageExtent.height = height;
  create_info.imageArrayLayers = imageArrayLayers;
  create_info.imageUsage = imageUsage;
  create_info.presentMode = presentMode;

  /* Leave this way I want "presentable images associated with the swapchain will own all of the pixels they contain" */
  create_info.clipped = VK_FALSE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  /* current transform should be applied to images in the swap chain */
  create_info.preTransform = (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? \
     VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : capabilities.currentTransform;

  for (uint8_t i = 0; i < ARR_LEN(ca_flags); i++) {
    if (capabilities.supportedCompositeAlpha & ca_flags[i]) {
      create_info.compositeAlpha = ca_flags[i];
      break;
    }
  }

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

  res = vkCreateSwapchainKHR(app->device, &create_info, NULL, &app->sc_data[cur_scd].swap_chain);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateSwapchainKHR"); }

  return res;
}

VkResult wlu_create_image_views(wlu_image_view_type type, vkcomp *app, uint32_t cur_index, VkImageViewCreateInfo *img_view_info) {
  VkResult res = VK_RESULT_MAX_ENUM;

  switch (type) {
    case WLU_SC_IMAGE_VIEWS:
      {
        VkImage *imgs = NULL;

        if (!app->sc_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_SC_DATA"); return res; }
        if (!app->sc_data[cur_index].swap_chain) { PERR(WLU_VKCOMP_SC, 0, NULL); return res; }

        /**
        * It's okay to reuse app->sc_data[cur_scd].sic. It'll give same result as minImageCount + 1.
        * Removal of function will result in validation layer errors
        */
        res = vkGetSwapchainImagesKHR(app->device, app->sc_data[cur_index].swap_chain, &app->sc_data[cur_index].sic, NULL);
        if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkGetSwapchainImagesKHR"); return res; }

        imgs = (VkImage *) alloca(app->sc_data[cur_index].sic * sizeof(VkImage));

        res = vkGetSwapchainImagesKHR(app->device, app->sc_data[cur_index].swap_chain, &app->sc_data[cur_index].sic, imgs);
        if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkGetSwapchainImagesKHR"); return res; }

        for (uint32_t i = 0; i < app->sc_data[cur_index].sic; i++) {
          img_view_info->image = app->sc_data[cur_index].sc_buffs[i].image = imgs[i];
          res = vkCreateImageView(app->device, img_view_info, NULL, &app->sc_data[cur_index].sc_buffs[i].view);
          if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateImageView"); return res; }
        }
      }

      break;
    case WLU_TEXT_IMAGE_VIEWS:
      if (!app->text_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_TEXT_DATA"); return res; }

      /**
      * Could set the image inside the VkImageViewCreateInfo struct,
      * but for reduncy and to ensure the image is correct assigning it here.
      */
      img_view_info->image = app->text_data[cur_index].image;
      res = vkCreateImageView(app->device, img_view_info, NULL, &app->text_data[cur_index].view);
      if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateImageView"); return res; }

      break;
    default: break;
  }

  return res;
}

VkResult wlu_create_depth_buff(
  vkcomp *app,
  uint32_t cur_scd,
  VkImageCreateInfo *img_info,
  VkImageViewCreateInfo *img_view_info,
  VkFlags requirements_mask
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (img_view_info->format == VK_FORMAT_D16_UNORM_S8_UINT || img_view_info->format == VK_FORMAT_D24_UNORM_S8_UINT || img_view_info->format == VK_FORMAT_D32_SFLOAT_S8_UINT)
    img_view_info->subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

  /* Create image object */
  res = vkCreateImage(app->device, img_info, NULL, &app->sc_data[cur_scd].depth.image);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateImage"); return res; }

  /**
  * Although you know the width, height, and the size of a buffer element,
  * there is no way to determine exactly how much memory is needed to allocate.
  * This is because alignment constraints that may be placed by the GPU hardware.
  * This function allows you to find out everything you need to allocate the
  * memory for an image.
  */
  VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(app->device, app->sc_data[cur_scd].depth.image, &mem_reqs);

  VkMemoryAllocateInfo mem_alloc = {};
  mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_alloc.pNext = NULL;
  mem_alloc.allocationSize = mem_reqs.size;
  mem_alloc.memoryTypeIndex = 0;

  VkBool32 pass; /* find a suitable memory type for depth bufffer */
  pass = memory_type_from_properties(app, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);
  if (!pass) {
    wlu_log_me(WLU_DANGER, "[x] memory_type_from_properties failed");
    return pass;
  }

  /* Allocate memory */
  res = vkAllocateMemory(app->device, &mem_alloc, NULL, &app->sc_data[cur_scd].depth.mem);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkAllocateMemory"); return res; }

  /* Associate memory with image object by binding */
  res = vkBindImageMemory(app->device, app->sc_data[cur_scd].depth.image, app->sc_data[cur_scd].depth.mem, 0);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkBindImageMemory"); return res; }

  /* Create an image view object for depth buffer */
  img_view_info->image = app->sc_data[cur_scd].depth.image;
  res = vkCreateImageView(app->device, img_view_info, NULL, &app->sc_data[cur_scd].depth.view);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkCreateImageView")

  return res;
}

VkResult wlu_create_vk_buffer(
  vkcomp *app,
  uint32_t cur_bd,
  VkDeviceSize size,
  VkBufferCreateFlagBits flags,
  VkBufferUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  char buff_name,
  VkFlags requirements_mask
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->buff_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_buff_data"); return res; }

  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = sharingMode;
  create_info.queueFamilyIndexCount = queueFamilyIndexCount;
  create_info.pQueueFamilyIndices = pQueueFamilyIndices;

  app->buff_data[cur_bd].name = buff_name;
  res = vkCreateBuffer(app->device, &create_info, NULL, &app->buff_data[cur_bd].buff);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateBuffer"); return res; }

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(app->device, app->buff_data[cur_bd].buff, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.allocationSize = app->buff_data[cur_bd].size = mem_reqs.size;
  alloc_info.memoryTypeIndex = 0;

  /* find a suitable memory type for VkBuffer */
  res = memory_type_from_properties(app, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (!res) { PERR(WLU_MEM_TYPE_ERR, 0, NULL) return res; }

  res = vkAllocateMemory(app->device, &alloc_info, NULL, &app->buff_data[cur_bd].mem);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkAllocateMemory"); return res; }

  /* associate the memory allocated with the buffer object */
  res = vkBindBufferMemory(app->device, app->buff_data[cur_bd].buff, app->buff_data[cur_bd].mem, 0);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkBindBufferMemory")

  return res;
}

VkResult wlu_create_buff_mem_map(
  vkcomp *app,
  uint32_t cur_bd,
  void *data
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->buff_data[cur_bd].mem) { PERR(WLU_VKCOMP_BUFF_MEM, 0, NULL); return res; }

  /**
  * Can Find in vulkan SDK doc/tutorial/html/07-init_uniform_buffer.html
  * With any buffer, you need to populate it with the data that
  * you want the shader to read. In order to get CPU access to
  * the memory, you need to map it
  */
  void *p_data = NULL;
  res = vkMapMemory(app->device, app->buff_data[cur_bd].mem, 0, app->buff_data[cur_bd].size, 0, &p_data);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkMapMemory"); return res; }
  if (data) memmove(p_data, data, app->buff_data[cur_bd].size);

  vkUnmapMemory(app->device, app->buff_data[cur_bd].mem);

  return res;
}

VkResult wlu_create_framebuffers(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t cur_gpd,
  uint32_t attachmentCount,
  VkImageView *pAttachments,
  uint32_t width,
  uint32_t height,
  uint32_t layers
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->gp_data[cur_gpd].render_pass) { PERR(WLU_VKCOMP_RENDER_PASS, 0, NULL); return res; }
  if (!app->sc_data[cur_scd].sc_buffs) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_SC_DATA_MEMS"); return res; }

  VkFramebufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  create_info.renderPass = app->gp_data[cur_gpd].render_pass;
  create_info.attachmentCount = attachmentCount;
  create_info.width = width;
  create_info.height = height;
  create_info.layers = layers;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    pAttachments[0] = app->sc_data[cur_scd].sc_buffs[i].view;
    create_info.pAttachments = pAttachments;

    res = vkCreateFramebuffer(app->device, &create_info, NULL, &app->sc_data[cur_scd].sc_buffs[i].fb);
    if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateFramebuffer"); return res; }
  }

  return res;
}

VkResult wlu_create_cmd_pool(
  vkcomp *app,
  uint32_t cur_scd,
  uint32_t cur_cmdd,
  uint32_t queueFamilyIndex,
  VkCommandPoolCreateFlagBits flags
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->sc_data[cur_scd].sic == 0) { PERR(WLU_VKCOMP_SC_IC, 0, NULL); return res; }
  if (!app->cmd_data) { PERR(WLU_BUFF_NOT_ALLOC, 0, "WLU_CMD_DATA"); return res; }

  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueFamilyIndex = queueFamilyIndex;

  res = vkCreateCommandPool(app->device, &create_info, NULL, &app->cmd_data[cur_cmdd].cmd_pool);
  if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateCommandPool"); return res; }

  return res;
}

VkResult wlu_create_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferLevel level
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->sc_data[cur_scd].sic == 0) { PERR(WLU_VKCOMP_SC_IC, 0, NULL); return res; }
  if (!app->cmd_data[cur_pool].cmd_pool) { PERR(WLU_VKCOMP_CMD_POOL, 0, NULL); return res; }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.commandPool = app->cmd_data[cur_pool].cmd_pool;
  alloc_info.level = level;
  alloc_info.commandBufferCount = app->sc_data[cur_scd].sic;

  res = vkAllocateCommandBuffers(app->device, &alloc_info, app->cmd_data[cur_pool].cmd_buffs);
  if (res) PERR(WLU_VK_FUNC_ERR, res, "vkAllocateCommandBuffers")

  return res;
}

/**
* (This comment is for me)
* Use a image semaphore to signal that an image
* has been acquired and is ready for rendering.
* Use a render semaphore to signal that rendering
* Use fence to synchronize application with rendering operation
* has finished and presentation can happen.
*/
VkResult wlu_create_syncs(vkcomp *app, uint32_t cur_scd) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkSemaphoreCreateInfo sem_info = {};
  sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  sem_info.pNext = NULL;
  sem_info.flags = 0;

  /* Queue the command buffer for execution */
  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.pNext = NULL;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < app->sc_data[cur_scd].sic; i++) {
    res = vkCreateSemaphore(app->device, &sem_info, NULL, &app->sc_data[cur_scd].syncs[i].sem.image);
    if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateSemaphore"); return res; }

    res = vkCreateSemaphore(app->device, &sem_info, NULL, &app->sc_data[cur_scd].syncs[i].sem.render);
    if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateSemaphore"); return res; }

    res = vkCreateFence(app->device, &fence_info, NULL, &app->sc_data[cur_scd].syncs[i].fence.render);
    if (res) { PERR(WLU_VK_FUNC_ERR, res, "vkCreateFence"); return res; }
  }

  return res;
}
