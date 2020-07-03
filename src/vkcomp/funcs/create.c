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

VkResult dlu_create_instance(
  vkcomp *app,
  char *app_name,
  char *engine_name,
  uint32_t enabledLayerCount,
  const char **ppEnabledLayerNames,
  uint32_t enabledExtensionCount,
  const char **ppEnabledExtensionNames
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

  /* enable validation layers best practice */
  VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
  VkValidationFeaturesEXT features = {};
  features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
  features.enabledValidationFeatureCount = ARR_LEN(enables);
  features.pEnabledValidationFeatures = enables;

  /**
  * Tells Vulkan which instance extensions
  * and validation layers we want to use
  */
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = (ppEnabledLayerNames) ? &features : NULL;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = enabledLayerCount;
  create_info.ppEnabledLayerNames = ppEnabledLayerNames;
  create_info.enabledExtensionCount = enabledExtensionCount;
  create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;

  /* Create the instance */
  res = vkCreateInstance(&create_info, NULL, &app->instance);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkCreateInstance")

  return res;
}

VkResult dlu_create_vkwayland_surfaceKHR(vkcomp *app, void *wl_display, void *wl_surface) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->instance) { PERR(DLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = (struct wl_display *) wl_display;
  create_info.surface = (struct wl_surface *) wl_surface;

  res = vkCreateWaylandSurfaceKHR(app->instance, &create_info, NULL, &app->surface);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkCreateWaylandSurfaceKHR")

  return res;
}

VkResult dlu_create_physical_device(
  vkcomp *app,
  uint32_t cur_pd,
  VkPhysicalDeviceType vkpdtype,
  VkPhysicalDeviceProperties *device_props,
  VkPhysicalDeviceFeatures *device_feats
) {

  VkResult res = VK_RESULT_MAX_ENUM;
  VkPhysicalDevice *devices = VK_NULL_HANDLE;
  uint32_t device_count = 0;

  if (!app->pd_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_PD_DATA"); return res; }
  if (!app->instance) { PERR(DLU_VKCOMP_INSTANCE, 0, NULL); return res; }

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, NULL);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEnumeratePhysicalDevices"); return res; }

  if (device_count == 0) {
    dlu_log_me(DLU_DANGER, "[x] failed to find GPU with Vulkan support!!!");
    return VK_RESULT_MAX_ENUM;
  }

  devices = (VkPhysicalDevice *) alloca(device_count * sizeof(VkPhysicalDevice));

  res = vkEnumeratePhysicalDevices(app->instance, &device_count, devices);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkEnumeratePhysicalDevices"); return res; }

  /**
  * get a physical device that is suitable
  * to do the graphics related task that we need
  */
  for (uint32_t i = 0; i < device_count; i++) {
    vkGetPhysicalDeviceProperties(devices[i], device_props); /* Query device properties */
    vkGetPhysicalDeviceFeatures(devices[i], device_feats); /* Query device features */
    if (device_props->deviceType == vkpdtype) {
      memmove(&app->pd_data[cur_pd].phys_dev, &devices[i], sizeof(devices[i]));
      dlu_log_me(DLU_SUCCESS, "Suitable GPU Found: %s", device_props->deviceName);
      break;
    }
  }

  if (app->pd_data[cur_pd].phys_dev == VK_NULL_HANDLE) {
    dlu_log_me(DLU_DANGER, "[x] failed to find a suitable GPU!!!");
    return VK_RESULT_MAX_ENUM;
  }

  return res;
}

VkBool32 dlu_create_queue_families(vkcomp *app, uint32_t cur_pd, VkQueueFlagBits vkqfbits) {
  VkBool32 ret = VK_TRUE;
  VkBool32 present_support = VK_FALSE;
  VkQueueFamilyProperties *queue_families = NULL;
  uint32_t qfc = 0; /* queue family count */

  if (!app->pd_data[cur_pd].phys_dev) { PERR(DLU_VKCOMP_PHYS_DEV, 0, NULL); return ret; }

  vkGetPhysicalDeviceQueueFamilyProperties(app->pd_data[cur_pd].phys_dev, &qfc, NULL);

  queue_families = (VkQueueFamilyProperties *) alloca(qfc * sizeof(VkQueueFamilyProperties));

  vkGetPhysicalDeviceQueueFamilyProperties(app->pd_data[cur_pd].phys_dev, &qfc, queue_families);

  for (uint32_t i = 0; i < qfc; i++) {
    if (queue_families[i].queueFlags & vkqfbits) {
      if (app->surface && !present_support) {
        /* Allows for the checking of presentation support and if a given queue family supports swap chains */
        vkGetPhysicalDeviceSurfaceSupportKHR(app->pd_data[cur_pd].phys_dev, i, app->surface, &present_support);

        /* Check to see if a device has support for the grapphics bit and if if can present images onto a surface */
        if (vkqfbits & VK_QUEUE_GRAPHICS_BIT && app->pd_data[cur_pd].gfam_idx == UINT32_MAX && present_support) {
          /* Retrieve Graphics/Present Family Queue index */
          app->pd_data[cur_pd].gfam_idx = i; ret = VK_FALSE;
          dlu_log_me(DLU_SUCCESS, "Physical Device Queue Family Index %d supports graphics operations", i);
          dlu_log_me(DLU_SUCCESS, "Physical Device Queue Family Index %d supports presentation to a given surface", i);
        }

        present_support = VK_FALSE;
      }

      if (vkqfbits & VK_QUEUE_COMPUTE_BIT && app->pd_data[cur_pd].cfam_idx == UINT32_MAX) {
        /* Retrieve Compute Family Queue index */
        app->pd_data[cur_pd].cfam_idx = i; ret = VK_FALSE;
        dlu_log_me(DLU_SUCCESS, "Physical Device Queue Family Index %d has support for commute operations", i);
      }

      if (vkqfbits & VK_QUEUE_TRANSFER_BIT && app->pd_data[cur_pd].tfam_idx == UINT32_MAX) {
        /* Retrieve Transfer Family Queue index */
        app->pd_data[cur_pd].tfam_idx = i; ret = VK_FALSE;
        dlu_log_me(DLU_SUCCESS, "Physical Device Queue Family Index %d has support for transfer operations", i);
      }
    }
  }

  return ret;
}

VkResult dlu_create_logical_device(
  vkcomp *app,
  uint32_t cur_pd,
  uint32_t cur_ld,
  VkDeviceCreateFlags flags,
  uint32_t queueCreateInfoCount,
  const VkDeviceQueueCreateInfo *pQueueCreateInfos,
  VkPhysicalDeviceFeatures *pEnabledFeatures,
  uint32_t enabledExtensionCount,
  const char *const *ppEnabledExtensionNames
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->ld_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_LD_DATA"); return res; }  
  if (!app->pd_data[cur_pd].phys_dev) { PERR(DLU_VKCOMP_PHYS_DEV, 0, NULL); return res; }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueCreateInfoCount = queueCreateInfoCount;
  create_info.pQueueCreateInfos = pQueueCreateInfos;
  create_info.enabledLayerCount = 0; // Deprecated and ignored
  create_info.ppEnabledLayerNames = NULL; // Deprecated and ignored 
  create_info.enabledExtensionCount = enabledExtensionCount;
  create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;
  create_info.pEnabledFeatures = pEnabledFeatures;

  /* Create logic device */
  res = vkCreateDevice(app->pd_data[cur_pd].phys_dev, &create_info, NULL, &app->ld_data[cur_ld].device);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateDevice"); return res; }

  /* Associate a logical device with a given physical */
  app->ld_data[cur_ld].pdi = cur_pd;

  return res;
}

VkBool32 dlu_create_device_queue(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t queueIndex,
  VkQueueFlagBits vkqfbits
) {

  if (!app->ld_data[cur_ld].device) { PERR(DLU_VKCOMP_DEVICE, 0, NULL); return VK_TRUE; }
  if (app->ld_data[cur_ld].pdi == UINT32_MAX) { PERR(DLU_VKCOMP_DEVICE_NOT_ASSOC, 0, "dlu_create_logical_device()"); return VK_TRUE; }

  /**
  * Queues are automatically created with the logical device, but you need a
  * VkQueue handle to interface with them
  */
  if (vkqfbits & VK_QUEUE_GRAPHICS_BIT) {
    vkGetDeviceQueue(app->ld_data[cur_ld].device, app->pd_data[app->ld_data[cur_ld].pdi].gfam_idx, queueIndex, &app->ld_data[cur_ld].graphics);
    if (!app->ld_data[cur_ld].graphics)  { dlu_log_me(DLU_DANGER, "[x] Failed to get graphics queue handle"); return VK_TRUE; }
  }

  if (vkqfbits & VK_QUEUE_COMPUTE_BIT) {
    vkGetDeviceQueue(app->ld_data[cur_ld].device, app->pd_data[app->ld_data[cur_ld].pdi].cfam_idx, queueIndex, &app->ld_data[cur_ld].compute);
    if (!app->ld_data[cur_ld].compute)  { dlu_log_me(DLU_DANGER, "[x] Failed to get compute queue handle"); return VK_TRUE; }
  }

  if (vkqfbits & VK_QUEUE_TRANSFER_BIT) {
    vkGetDeviceQueue(app->ld_data[cur_ld].device, app->pd_data[app->ld_data[cur_ld].pdi].tfam_idx, queueIndex, &app->ld_data[cur_ld].transfer);
    if (!app->ld_data[cur_ld].transfer)  { dlu_log_me(DLU_DANGER, "[x] Failed to get transfer queue handle"); return VK_TRUE; }
  }

  return VK_FALSE;
}

VkResult dlu_create_swap_chain(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_scd,
  VkSwapchainCreateInfoKHR *create_info
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->surface) { PERR(DLU_VKCOMP_SURFACE, 0, NULL); return res; }
  if (!app->sc_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA"); return res; }
  if (!app->ld_data[cur_ld].device) { PERR(DLU_VKCOMP_DEVICE, 0, NULL); return res; }

  VkCompositeAlphaFlagBitsKHR ca_flags[4] = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
  };

  /* current transform should be applied to images in the swap chain */
  create_info->preTransform = (create_info->preTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? \
     VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : create_info->preTransform;

  for (uint8_t i = 0; i < ARR_LEN(ca_flags); i++) {
    if (create_info->compositeAlpha & ca_flags[i]) {
      create_info->compositeAlpha = ca_flags[i];
      break;
    }
  }

  res = vkCreateSwapchainKHR(app->ld_data[cur_ld].device, create_info, NULL, &app->sc_data[cur_scd].swap_chain);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateSwapchainKHR"); }

  /* Associate a swapchain with a given VkDevice */
  app->sc_data[cur_scd].ldi = cur_ld;

  return res;
}

VkResult dlu_create_image_views(dlu_image_view_type type, vkcomp *app, uint32_t cur_idx, VkImageViewCreateInfo *ivi) {
  VkResult res = VK_RESULT_MAX_ENUM;

  switch (type) {
    case DLU_SC_IMAGE_VIEWS:
      {
        VkImage *imgs = NULL;

        if (!app->sc_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA"); return res; }
        if (!app->sc_data[cur_idx].swap_chain) { PERR(DLU_VKCOMP_SC, 0, NULL); return res; }
        if (app->sc_data[cur_idx].ldi == UINT32_MAX) { PERR(DLU_VKCOMP_DEVICE_NOT_ASSOC, 0, "dlu_create_swap_chain()"); return res; }

        /**
        * It's okay to reuse app->sc_data[cur_scd].sic. It'll give same result as minImageCount + 1.
        * Removal of function will result in validation layer errors
        */
        res = vkGetSwapchainImagesKHR(app->ld_data[app->sc_data[cur_idx].ldi].device, app->sc_data[cur_idx].swap_chain, &app->sc_data[cur_idx].sic, NULL);
        if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkGetSwapchainImagesKHR"); return res; }

        imgs = (VkImage *) alloca(app->sc_data[cur_idx].sic * sizeof(VkImage));

        res = vkGetSwapchainImagesKHR(app->ld_data[app->sc_data[cur_idx].ldi].device, app->sc_data[cur_idx].swap_chain, &app->sc_data[cur_idx].sic, imgs);
        if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkGetSwapchainImagesKHR"); return res; }

        for (uint32_t i = 0; i < app->sc_data[cur_idx].sic; i++) {
          ivi->image = app->sc_data[cur_idx].sc_buffs[i].image = imgs[i];
          res = vkCreateImageView(app->ld_data[app->sc_data[cur_idx].ldi].device, ivi, NULL, &app->sc_data[cur_idx].sc_buffs[i].view);
          if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateImageView"); return res; }
        }
      }

      break;
    case DLU_TEXT_IMAGE_VIEWS:
      {
        if (!app->text_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_TEXT_DATA"); return res; }
        if (app->text_data[cur_idx].ldi == UINT32_MAX) { PERR(DLU_VKCOMP_DEVICE_NOT_ASSOC, 0, "dlu_create_texture_image()"); return res; }

        /**
        * Could set the image inside the VkImageViewCreateInfo struct,
        * but for reduncy and to ensure the image is correct assigning it here.
        */
        ivi->image = app->text_data[cur_idx].image;
        res = vkCreateImageView(app->ld_data[app->text_data[cur_idx].ldi].device, ivi, NULL, &app->text_data[cur_idx].view);
        if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateImageView"); return res; }
      }
      break;
    case DLU_DEPTH_IMAGE_VIEWS:
      {
        if (!app->sc_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA"); return res; }
        if (app->sc_data[cur_idx].ldi == UINT32_MAX) { PERR(DLU_VKCOMP_DEVICE_NOT_ASSOC, 0, "dlu_create_swap_chain()"); return res; }
        /* ADD Depth Buffer Checking Here */

        if (ivi->format == VK_FORMAT_D16_UNORM_S8_UINT || ivi->format == VK_FORMAT_D24_UNORM_S8_UINT || ivi->format == VK_FORMAT_D32_SFLOAT_S8_UINT)
          ivi->subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

        /* Create an image view object for depth buffer */
        ivi->image = app->sc_data[cur_idx].depth.image;
        res = vkCreateImageView(app->ld_data[app->sc_data[cur_idx].ldi].device, ivi, NULL, &app->sc_data[cur_idx].depth.view);
        if (res) PERR(DLU_VK_FUNC_ERR, res, "vkCreateImageView")
      }
      break;
    default: break;
  }

  return res;
}

VkResult dlu_create_depth_buff(
  vkcomp *app,
  uint32_t cur_scd,
  VkImageCreateInfo *img_info,
  VkFlags requirements_mask
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->sc_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA"); return res; }
  if (app->sc_data[cur_scd].ldi == UINT32_MAX) { PERR(DLU_VKCOMP_DEVICE_NOT_ASSOC, 0, "dlu_create_swap_chain()"); return res; }

  /* Create image object */
  res = vkCreateImage(app->ld_data[app->sc_data[cur_scd].ldi].device, img_info, NULL, &app->sc_data[cur_scd].depth.image);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateImage"); return res; }

  /**
  * Although you know the width, height, and the size of a buffer element,
  * there is no way to determine exactly how much memory is needed to allocate.
  * This is because alignment constraints that may be placed by the GPU hardware.
  * This function allows you to find out everything you need to allocate the
  * memory for an image.
  */
  VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(app->ld_data[app->sc_data[cur_scd].ldi].device, app->sc_data[cur_scd].depth.image, &mem_reqs);

  VkMemoryAllocateInfo mem_alloc = {};
  mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_alloc.pNext = NULL;
  mem_alloc.allocationSize = mem_reqs.size;
  mem_alloc.memoryTypeIndex = 0;

  VkBool32 pass; /* find a suitable memory type for depth bufffer */
  pass = memory_type_from_properties(app, app->ld_data[app->sc_data[cur_scd].ldi].pdi, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);
  if (!pass) {
    dlu_log_me(DLU_DANGER, "[x] memory_type_from_properties failed");
    return pass;
  }

  /* Allocate memory */
  res = vkAllocateMemory(app->ld_data[app->sc_data[cur_scd].ldi].device, &mem_alloc, NULL, &app->sc_data[cur_scd].depth.mem);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkAllocateMemory"); return res; }

  /**
  * Associate the memory allocated with the VkBuffer resource.
  * It is easier to attach the entire VkDeviceMemory to the VkBuffer resource
  * So offset will always be zero whenever a call to vkBind*Memory is called
  */
  res = vkBindImageMemory(app->ld_data[app->sc_data[cur_scd].ldi].device, app->sc_data[cur_scd].depth.image, app->sc_data[cur_scd].depth.mem, 0);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkBindImageMemory")

  return res;
}

VkResult dlu_create_vk_buffer(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_bd,
  VkDeviceSize size,
  VkBufferCreateFlagBits flags,
  VkBufferUsageFlags usage,
  VkSharingMode sharingMode,
  uint32_t queueFamilyIndexCount,
  const uint32_t *pQueueFamilyIndices,
  VkFlags requirements_mask
) {

  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->buff_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_BUFF_DATA"); return res; }

  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = sharingMode;
  create_info.queueFamilyIndexCount = queueFamilyIndexCount;
  create_info.pQueueFamilyIndices = pQueueFamilyIndices;

  res = vkCreateBuffer(app->ld_data[cur_ld].device, &create_info, NULL, &app->buff_data[cur_bd].buff);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateBuffer"); return res; }

  /* Associate a buffer with a VkDevice */
  app->buff_data[cur_bd].ldi = cur_ld;

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(app->ld_data[cur_ld].device, app->buff_data[cur_bd].buff, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.allocationSize = app->buff_data[cur_bd].size = mem_reqs.size;
  alloc_info.memoryTypeIndex = 0;

  /* find a suitable memory type for VkBuffer */
  res = memory_type_from_properties(app, app->ld_data[cur_ld].pdi, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (!res) { PERR(DLU_MEM_TYPE_ERR, 0, NULL) return res; }

  res = vkAllocateMemory(app->ld_data[cur_ld].device, &alloc_info, NULL, &app->buff_data[cur_bd].mem);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkAllocateMemory"); return res; }

  /**
  * Associate the memory allocated with the VkBuffer resource.
  * It is easier to attach the entire VkDeviceMemory to the VkBuffer resource
  * So offset will always be zero whenever a call to vkBind*Memory is called
  */
  res = vkBindBufferMemory(app->ld_data[cur_ld].device, app->buff_data[cur_bd].buff, app->buff_data[cur_bd].mem, 0);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkBindBufferMemory")

  return res;
}

VkResult dlu_create_framebuffers(
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

  if (!app->gp_data[cur_gpd].render_pass) { PERR(DLU_VKCOMP_RENDER_PASS, 0, NULL); return res; }
  if (!app->sc_data[cur_scd].sc_buffs) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA_MEMS"); return res; }

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

    res = vkCreateFramebuffer(app->ld_data[app->sc_data[cur_scd].ldi].device, &create_info, NULL, &app->sc_data[cur_scd].sc_buffs[i].fb);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateFramebuffer"); return res; }
  }

  return res;
}

VkResult dlu_create_cmd_pool(
  vkcomp *app,
  uint32_t cur_ld,
  uint32_t cur_scd,
  uint32_t cur_cmdd,
  uint32_t queueFamilyIndex,
  VkCommandPoolCreateFlagBits flags
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->sc_data[cur_scd].sic == 0) { PERR(DLU_VKCOMP_SC_IC, 0, NULL); return res; }
  if (!app->cmd_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_CMD_DATA"); return res; }

  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueFamilyIndex = queueFamilyIndex;

  res = vkCreateCommandPool(app->ld_data[cur_ld].device, &create_info, NULL, &app->cmd_data[cur_cmdd].cmd_pool);
  if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateCommandPool"); return res; }

  /* Associate a command pool with a logical device */
  app->cmd_data[cur_cmdd].ldi = cur_ld;

  return res;
}

VkResult dlu_create_cmd_buffs(
  vkcomp *app,
  uint32_t cur_pool,
  uint32_t cur_scd,
  VkCommandBufferLevel level
) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (app->sc_data[cur_scd].sic == 0) { PERR(DLU_VKCOMP_SC_IC, 0, NULL); return res; }
  if (!app->cmd_data[cur_pool].cmd_pool) { PERR(DLU_VKCOMP_CMD_POOL, 0, NULL); return res; }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.commandPool = app->cmd_data[cur_pool].cmd_pool;
  alloc_info.level = level;
  alloc_info.commandBufferCount = app->sc_data[cur_scd].sic;

  res = vkAllocateCommandBuffers(app->ld_data[app->cmd_data[cur_pool].ldi].device, &alloc_info, app->cmd_data[cur_pool].cmd_buffs);
  if (res) PERR(DLU_VK_FUNC_ERR, res, "vkAllocateCommandBuffers")

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
VkResult dlu_create_syncs(vkcomp *app, uint32_t cur_scd) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->sc_data) { PERR(DLU_BUFF_NOT_ALLOC, 0, "DLU_SC_DATA"); return res; }

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
    res = vkCreateSemaphore(app->ld_data[app->sc_data[cur_scd].ldi].device, &sem_info, NULL, &app->sc_data[cur_scd].syncs[i].sem.image);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateSemaphore"); return res; }

    res = vkCreateSemaphore(app->ld_data[app->sc_data[cur_scd].ldi].device, &sem_info, NULL, &app->sc_data[cur_scd].syncs[i].sem.render);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateSemaphore"); return res; }

    res = vkCreateFence(app->ld_data[app->sc_data[cur_scd].ldi].device, &fence_info, NULL, &app->sc_data[cur_scd].syncs[i].fence.render);
    if (res) { PERR(DLU_VK_FUNC_ERR, res, "vkCreateFence"); return res; }
  }

  return res;
}
