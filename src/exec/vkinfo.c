/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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
#include <exec/vkinfo.h>

const char *device_extensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char *instance_extensions[] = {
  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  VK_KHR_SURFACE_EXTENSION_NAME,
  VK_KHR_DISPLAY_EXTENSION_NAME
};

static const char *colors[] = {
	[WLU_NONE]    = "",
	[WLU_SUCCESS] = "\x1B[32;1m",
	[WLU_DANGER]  = "\x1B[31;1m",
	[WLU_INFO]    = "\x1B[30;1m",
	[WLU_WARNING] = "\x1B[33;1m",
	[WLU_RESET]   = "\x1b[0m",
};

void help_message() {
  fprintf(stdout, "Usage: lucur [options]\n");
  fprintf(stdout, "Example: lucur --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU\n");

  fprintf(stdout, "Options:\n");
  fprintf(stdout, "\t-l, --pgvl\t\t\t Print global validation layers\n");
  fprintf(stdout, "\t-i, --pie\t\t\t Print instance extenstion list\n");
  fprintf(stdout, "\t-d, --pde <VkPhysicalDeviceType> Print device extenstion list\n");
  fprintf(stdout, "\t-v, --version\t\t\t Print lucurious library version\n");
  fprintf(stdout, "\t-h, --help\t\t\t Show this message\n");
}

void version_num() {
  fprintf(stdout, "%s", colors[WLU_SUCCESS]);
  fprintf(stdout, "lucurious %s\n", LUCUR_VERSION);
  fprintf(stdout, "%s", colors[WLU_RESET]);
}

void lower_to_upper(char *s) {
  while (*s != '\0') {
    if (*s >= 'a' && *s <= 'z')
      *s = ('A' + *s - 'a');
    s++;
  }
}

void print_gvalidation_layers() {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_set_global_layers(app);
  if (err) {
    wlu_freeup_vk(app);
    fprintf(stdout, "%s", colors[WLU_DANGER]);
    fprintf(stdout, "[x] Vulkan SDK must not be installed\n");
    fprintf(stdout, "[x] wlu_set_global_layers failed with error code: %d\n", err);
    fprintf(stdout, "%s\n", colors[WLU_RESET]);
    return;
  }

  fprintf(stdout, "%s", colors[WLU_SUCCESS]);
  fprintf(stdout, "\n\t\t\t\tValidation Layers List\n\t\tLayer Name\t\t\t\tDescription\n\n");
  fprintf(stdout, "%s", colors[WLU_INFO]);
  for (uint32_t i = 0; i < app->vk_layer_count; i++) {
    fprintf(stdout, "\t%s", app->vk_layer_props[i].layerName);
    fprintf(stdout, "\t\t%s\n", app->vk_layer_props[i].description);
  }
  fprintf(stdout, "%s", colors[WLU_WARNING]);
  fprintf(stdout, "\n\tValidation Layer Count: %d\n", app->vk_layer_count);
  fprintf(stdout, "%s\n", colors[WLU_RESET]);

  wlu_freeup_vk(app);
}

void print_instance_extensions() {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "PrintStmt", "PrintStmt", 0, NULL, 3, instance_extensions);
  if (err) {
    fprintf(stdout, "%s", colors[WLU_DANGER]);
    fprintf(stdout, "[x] Failed to create instance\n");
    fprintf(stdout, "%s\n", colors[WLU_RESET]);
    wlu_freeup_vk(app);
    return;
  }

  fprintf(stdout, "%s", colors[WLU_SUCCESS]);
  fprintf(stdout, "\n\t Instance Extension List\n  SpecVersion\t\tExtension Name\n\n");
  fprintf(stdout, "%s", colors[WLU_INFO]);
  for (uint32_t i = 0; i < app->ep_instance_count; i++) {
    lower_to_upper(app->ep_instance_props[i].extensionName);
    fprintf(stdout, "\t%d\t %s_EXTENSION_NAME\n",
            app->ep_instance_props[i].specVersion,
            app->ep_instance_props[i].extensionName);
  }
  fprintf(stdout, "%s", colors[WLU_WARNING]);
  fprintf(stdout, "\n  Instance Extension Count: %d\n", app->ep_instance_count);
  fprintf(stdout, "%s\n", colors[WLU_RESET]);

  wlu_freeup_vk(app);
}

void print_device_extensions(VkPhysicalDeviceType dt) {
  VkResult err;
  vkcomp *app = NULL;
  app = wlu_init_vk();

  err = wlu_create_instance(app, "PrintStmt", "PrintStmt", 0, NULL, 3, instance_extensions);
  if (err) {
    fprintf(stdout, "%s", colors[WLU_DANGER]);
    fprintf(stdout, "[x] Failed to create instance\n");
    fprintf(stdout, "%s\n", colors[WLU_RESET]);
    wlu_freeup_vk(app);
    return;
  }

  err = wlu_create_physical_device(app, dt);
  if (err) {
    fprintf(stdout, "%s", colors[WLU_DANGER]);
    fprintf(stdout, "[x] Failed to find physical device\n");
    fprintf(stdout, "%s\n", colors[WLU_RESET]);
    wlu_freeup_vk(app);
    return;
  }

  fprintf(stdout, "%s", colors[WLU_SUCCESS]);
  fprintf(stdout, "\n\t   Device Extension List\n  SpecVersion\t\tExtension Name\n\n");
  fprintf(stdout, "%s", colors[WLU_INFO]);
  for (uint32_t i = 0; i < app->ep_device_count; i++) {
    lower_to_upper(app->ep_device_props[i].extensionName);
    fprintf(stdout, "\t%d\t %s_EXTENSION_NAME\n",
            app->ep_device_props[i].specVersion,
            app->ep_device_props[i].extensionName);
  }
  fprintf(stdout, "%s", colors[WLU_WARNING]);
  printf("\n  Device Extension Count: %d\n", app->ep_device_count);
  fprintf(stdout, "%s\n", colors[WLU_RESET]);

  wlu_freeup_vk(app);
}
