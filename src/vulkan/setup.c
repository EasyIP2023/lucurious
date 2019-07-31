#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/utils/log.h>
#include <vlucur/devices.h>

static void set_values(vkcomp *app) {
  app->instance = VK_NULL_HANDLE;
  app->surface = VK_NULL_HANDLE;
  app->vk_layer_props = NULL;
  app->vk_layer_count = VK_NULL_HANDLE;
  app->ep_instance_props = NULL;
  app->ep_instance_count = VK_NULL_HANDLE;
  app->ep_device_props = NULL;
  app->ep_device_count = VK_NULL_HANDLE;
  // app->device_properties;
  // app->device_features;
  // app->memory_properties;
  app->physical_device = VK_NULL_HANDLE;
  app->queue_create_infos = NULL;
  app->queue_families = NULL;
  app->queue_family_count = VK_NULL_HANDLE;
  app->indices.graphics_family = UINT32_MAX;
  app->indices.present_family = UINT32_MAX;
  app->device = VK_FALSE;
  app->graphics_queue = VK_FALSE;
  app->sc_buffs = VK_NULL_HANDLE;
  app->swap_chain = VK_NULL_HANDLE;
  app->sc_buff_size = VK_NULL_HANDLE;
  app->pipeline_layout = VK_NULL_HANDLE;
  app->render_pass = VK_NULL_HANDLE;
  app->sc_frame_buffs = VK_NULL_HANDLE;
  app->cmd_pool = VK_NULL_HANDLE;
  app->cmd_buffs = VK_NULL_HANDLE;
}

vkcomp *wlu_init_vk() {
  vkcomp *app = calloc(sizeof(vkcomp), sizeof(vkcomp));
  if (!app) return NULL;
  set_values(app);
  return app;
}

/*
 * Gets all you're validation layers extensions
 * that comes installed with the vulkan sdk
 */
VkResult wlu_set_global_layers(vkcomp *app) {
  uint32_t layer_count = 0;
  VkLayerProperties *vk_props = NULL;
  VkResult res = VK_INCOMPLETE;

  do {
    res = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkEnumerateInstanceLayerProperties, ERROR CODE: %d", res);
      goto finish_vk_props;
    }

    /* layer count will only be zero if vulkan sdk not installed */
    if (layer_count == 0) {
      wlu_log_me(WLU_WARNING, "[x] failed to find Validation Layers, layer_count equals 0");
      goto finish_vk_props;
    }

    vk_props = (VkLayerProperties *) realloc(vk_props, layer_count * sizeof(VkLayerProperties));
    if (!vk_props) {
      res = VK_RESULT_MAX_ENUM;
      wlu_log_me(WLU_DANGER, "[x] realloc VkLayerProperties *vk_props failed");
      goto finish_vk_props;
    }

    res = vkEnumerateInstanceLayerProperties(&layer_count, vk_props);
  } while (res == VK_INCOMPLETE);

  app->vk_layer_props = (VkLayerProperties *) \
    calloc(sizeof(VkLayerProperties), layer_count * sizeof(VkLayerProperties));
  if (!app->vk_layer_props) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] calloc for app->vk_layer_props failed");
    goto finish_vk_props;
  }

  /* Gather the extension list for each instance layer. */
  for (uint32_t i = 0; i < layer_count; i++) {
    res = get_extension_properties(NULL, &vk_props[i], NULL);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] get_extension_properties failed, ERROR CODE: %d", res);
      goto finish_vk_props;
    }
    app->vk_layer_props[i] = vk_props[i];
    app->vk_layer_count = i;
  }

finish_vk_props:
  if (vk_props) {
    free(vk_props);
    vk_props = NULL;
  }
  return res;
}

/* Create connection between app and the vulkan api */
VkResult wlu_create_instance(vkcomp *app, char *app_name, char *engine_name) {
  VkResult res = VK_INCOMPLETE;

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
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = NULL;
  create_info.enabledExtensionCount = 3;
  create_info.ppEnabledExtensionNames = instance_extensions;

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
VkResult wlu_enumerate_devices(vkcomp *app, VkQueueFlagBits vkqfbits, VkPhysicalDeviceType vkpdtype) {
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
    if (is_device_suitable(app, devices[i], vkpdtype) &&
        find_queue_families(app, devices[i], vkqfbits) &&
        /* Check if current device has swap chain support */
        get_extension_properties(app, NULL, devices[i])) {
      app->physical_device = devices[i];
      /* Query device properties */
      vkGetPhysicalDeviceProperties(app->physical_device, &app->device_properties);
      wlu_log_me(WLU_SUCCESS, "Suitable GPU Found: %s", app->device_properties.deviceName);
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

/*
 * After selecting a physical device to use.
 *  Set up a logical device to interface with it
 */
VkResult wlu_create_logical_device(vkcomp *app) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkQueue present_queue;
  float queue_priorities[1] = {1.0};

  if (!app->physical_device) {
    wlu_log_me(WLU_DANGER, "[x] A physical device must be set");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_enumerate_devices(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->queue_create_infos = (VkDeviceQueueCreateInfo *) calloc(sizeof(VkDeviceQueueCreateInfo),
      app->queue_family_count * sizeof(VkDeviceQueueCreateInfo));
  if (!app->queue_create_infos) {
    wlu_log_me(WLU_DANGER, "[x] calloc app->queue_create_infos failed");
    return VK_RESULT_MAX_ENUM;
  }

  /* For creation of the presentation queue */
  for (uint32_t i = 0; i < app->queue_family_count; i++) {
    app->queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    app->queue_create_infos[i].pNext = NULL;
    app->queue_create_infos[i].flags = 0;
    app->queue_create_infos[i].queueFamilyIndex = i;
    app->queue_create_infos[i].queueCount = app->queue_family_count;
    app->queue_create_infos[i].pQueuePriorities = queue_priorities;
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
  create_info.pQueueCreateInfos = app->queue_create_infos;
  create_info.queueCreateInfoCount = app->queue_create_infos[0].queueCount;
  create_info.pEnabledFeatures = &app->device_features;
  create_info.enabledExtensionCount = 1;
  create_info.ppEnabledExtensionNames = device_extensions;
  create_info.ppEnabledLayerNames = NULL;
  create_info.enabledLayerCount = 0;

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
  vkGetDeviceQueue(app->device, app->indices.present_family, 0, &present_queue);

  return res;
}

VkResult wlu_create_swap_chain(
  vkcomp *app,
  VkSurfaceCapabilitiesKHR capabilities,
  VkSurfaceFormatKHR surface_fmt,
  VkPresentModeKHR pres_mode,
  VkExtent2D extent
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
  }

  /*
   * Don't want to stick to minimum becuase one would have to wait on the
   * drive to complete internal operations before one can acquire another
   * images to render to. So it's recommended to add one to minImageCount
   */
  app->sc_buff_size = capabilities.minImageCount + 1;

  /* Be sure sc_buff_size doesn't exceed the maximum. */
  if (capabilities.maxImageCount > 0 && app->sc_buff_size > capabilities.maxImageCount)
    app->sc_buff_size = capabilities.maxImageCount;

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
  create_info.flags = VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR;
  create_info.surface = app->surface;
  create_info.minImageCount = app->sc_buff_size;
  create_info.imageFormat = surface_fmt.format;
  create_info.imageColorSpace = surface_fmt.colorSpace;
  create_info.imageExtent.width = extent.width;
  create_info.imageExtent.height = extent.height;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  /* current transform should be applied to images in the swap chain */
  create_info.preTransform = pre_transform;
  /* specify that I currently do not want any transformation */
  create_info.compositeAlpha = composite_alpha;
  create_info.presentMode = pres_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (app->indices.graphics_family != app->indices.present_family) {
    const uint32_t queue_family_indices[2] = {
      app->indices.graphics_family,
      app->indices.present_family
    };
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = sizeof(queue_family_indices);
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;
  }

  res = vkCreateSwapchainKHR(app->device, &create_info, NULL, &app->swap_chain);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkCreateSwapchainKHR failed, ERROR CODE: %d", res);
    return res;
  }

  return res;
}

VkResult wlu_create_img_views(vkcomp *app, VkFormat format, VkImageViewType type) {
  VkResult res = VK_RESULT_MAX_ENUM;
  VkImage *sc_imgs = NULL;

  if (!app->swap_chain) {
    wlu_log_me(WLU_DANGER, "[x] Swap Chain doesn't exists");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_swap_chain(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    goto finish_create_img_views;
  }

  app->sc_buffs = (swap_chain_buffers *) calloc(sizeof(swap_chain_buffers),
      app->sc_buff_size * sizeof(swap_chain_buffers));
  if (!app->sc_buffs) {
    wlu_log_me(WLU_DANGER, "[x] calloc app->sc_buffs failed");
    goto finish_create_img_views;
  }

  res = vkGetSwapchainImagesKHR(app->device, app->swap_chain, &app->sc_buff_size, NULL);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkGetSwapchainImagesKHR failed, ERROR CODE: %d", res);
    goto finish_create_img_views;
  }

  sc_imgs = (VkImage *) calloc(sizeof(VkImage), app->sc_buff_size * sizeof(VkImage));
  if (!sc_imgs) {
    res = VK_RESULT_MAX_ENUM;
    wlu_log_me(WLU_DANGER, "[x] calloc VkImage *sc_imgs failed");
    goto finish_create_img_views;
  }

  res = vkGetSwapchainImagesKHR(app->device, app->swap_chain, &app->sc_buff_size, sc_imgs);
  if (res) {
    wlu_log_me(WLU_DANGER, "[x] vkGetSwapchainImagesKHR failed, ERROR CODE: %d", res);
    goto finish_create_img_views;
  }

  for (uint32_t i = 0; i < app->sc_buff_size; i++) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = app->sc_buffs[i].image = sc_imgs[i];
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

    res = vkCreateImageView(app->device, &create_info, NULL, &app->sc_buffs[i].view);
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

/*
 * This function creates the framebuffers
 * Attachments specified when creating the render pass
 * are bounded by wrapping them into a VkFramebuffer object.
 * A framebuffer object references all VkImageView objects this
 * is represented by "attachments"
 */
VkResult wlu_create_framebuffers(vkcomp *app, uint32_t attachment_count, VkExtent2D extent, uint32_t layers) {
  VkResult res = VK_RESULT_MAX_ENUM;

  if (!app->render_pass) {
    wlu_log_me(WLU_DANGER, "[x] render pass not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_render_pass(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  if (!app->sc_buffs) {
    wlu_log_me(WLU_DANGER, "[x] Swap Chain buffers not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_create_img_views(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return res;
  }

  app->sc_frame_buffs = (VkFramebuffer *) calloc(sizeof(VkFramebuffer),
        app->sc_buff_size * sizeof(VkFramebuffer));
  if (!app->sc_frame_buffs) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkFramebuffer *sc_frame_buffs failed");
    return res;
  }

  VkImageView attachments[app->sc_buff_size];

  for (uint32_t i = 0; i < app->sc_buff_size; i++) {
    attachments[i] = app->sc_buffs[i].view;

    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = app->render_pass;
    create_info.attachmentCount = attachment_count;
    create_info.pAttachments = attachments;
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = layers;

    res = vkCreateFramebuffer(app->device, &create_info, NULL, &app->sc_frame_buffs[i]);
    if (res) {
      wlu_log_me(WLU_DANGER, "[x] vkCreateFramebuffer failed, ERROR CODE: %d", res);
      return res;
    }
  }

  wlu_log_me(WLU_SUCCESS, "Frame Buffers have been successfully created");

  return res;
}

VkResult wlu_create_cmd_pool(vkcomp *app, VkCommandPoolCreateFlagBits flags) {
  VkResult res = VK_RESULT_MAX_ENUM;

  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = flags;
  create_info.queueFamilyIndex = app->indices.graphics_family;

  res = vkCreateCommandPool(app->device, &create_info, NULL, &app->cmd_pool);

  return res;
}

VkResult wlu_create_cmd_buffs(vkcomp *app, VkCommandBufferLevel level) {
  VkResult res = VK_RESULT_MAX_ENUM;

  app->cmd_buffs = (VkCommandBuffer * ) calloc(sizeof(VkCommandBuffer),
        app->sc_buff_size * sizeof(VkCommandBuffer));
  if (!app->cmd_buffs) {
    wlu_log_me(WLU_DANGER, "[x] calloc VkCommandBuffer *cmd_buffs failed");
    return res;
  }

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.commandPool = app->cmd_pool;
  alloc_info.level = level;
  alloc_info.commandBufferCount = (uint32_t) app->sc_buff_size;

  res = vkAllocateCommandBuffers(app->device, &alloc_info, app->cmd_buffs);

  return res;
}

void wlu_freeup_vk(void *data) {
  vkcomp *app = (vkcomp *) data;

  if (app->vk_layer_props)
    free(app->vk_layer_props);
  if (app->ep_instance_props)
    free(app->ep_instance_props);
  if (app->ep_device_props)
    free(app->ep_device_props);
  if (app->queue_families)
    free(app->queue_families);
  if (app->queue_create_infos)
    free(app->queue_create_infos);
  if (app->cmd_buffs)
    vkFreeCommandBuffers(app->device, app->cmd_pool, app->sc_buff_size, app->cmd_buffs);
  if (app->cmd_pool)
    vkDestroyCommandPool(app->device, app->cmd_pool, NULL);
  if (app->sc_frame_buffs) {
    for (uint32_t i = 0; i < app->sc_buff_size; i++) {
      vkDestroyFramebuffer(app->device, app->sc_frame_buffs[i], NULL);
      app->sc_frame_buffs[i] = VK_NULL_HANDLE;
    }
    free(app->sc_frame_buffs);
  }
  if (app->graphics_pipeline)
    vkDestroyPipeline(app->device, app->graphics_pipeline, NULL);
  if (app->pipeline_layout)
    vkDestroyPipelineLayout(app->device, app->pipeline_layout, NULL);
  if (app->render_pass)
    vkDestroyRenderPass(app->device, app->render_pass, NULL);
  if (app->sc_buffs) {
    for (uint32_t i = 0; i < app->sc_buff_size; i++) {
      vkDestroyImageView(app->device, app->sc_buffs[i].view, NULL);
      app->sc_buffs[i].view = VK_NULL_HANDLE;
    }
    free(app->sc_buffs);
  }
  if (app->swap_chain)
    vkDestroySwapchainKHR(app->device, app->swap_chain, NULL);
  if (app->device) {
    vkDeviceWaitIdle(app->device);
    vkDestroyDevice(app->device, NULL);
  }
  if (app->surface)
    vkDestroySurfaceKHR(app->instance, app->surface, NULL);
  if (app->instance)
    vkDestroyInstance(app->instance, NULL);

  set_values(app);
  if (app)
    free(app);
  app = NULL;
}
