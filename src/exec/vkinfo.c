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

static const char *instance_extensions[] = {
  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
  VK_KHR_SURFACE_EXTENSION_NAME,
  VK_KHR_DISPLAY_EXTENSION_NAME
};

void lower_to_upper(char *s);

void print_gvalidation_layers() {
  VkResult err;
  vkcomp *app = wlu_init_vk();

  VkLayerProperties *vk_props = VK_NULL_HANDLE;
  uint32_t lcount = 0;
 
  err = wlu_set_global_layers(&vk_props, &lcount);
  if (err) {
    wlu_freeup_vk(app);
    wlu_print_msg(WLU_DANGER, "[x] Vulkan SDK must not be installed\n");
    wlu_print_msg(WLU_DANGER, "[x] wlu_set_global_layers failed\n");
    return;
  }

  wlu_print_msg(WLU_SUCCESS, "\n\t\t\t\tValidation Layers List\n\t\tLayer Name\t\t\t\tDescription\n\n");

  for (uint32_t i = 0; i < lcount; i++) {
    wlu_print_msg(WLU_INFO, "\t%s", vk_props[i].layerName);
    wlu_print_msg(WLU_INFO, "\t\t%s\n", vk_props[i].description);
  }

  wlu_print_msg(WLU_WARNING, "\n\tValidation Layer Count: %d\n", lcount);

  wlu_freeup_vk(app);
}

void print_instance_extensions() {
  VkResult err;
  vkcomp *app = wlu_init_vk();

  err = wlu_create_instance(app, "PrintStmt", "PrintStmt", 0, NULL, 3, instance_extensions);
  if (err) { wlu_freeup_vk(app); return; }

  wlu_print_msg(WLU_SUCCESS, "\n\t Instance Extension List\n  SpecVersion\t\tExtension Name\n\n");

  /* set available instance extensions */
  VkExtensionProperties *ie_props = VK_NULL_HANDLE;
  uint32_t eip_count = 0;

  err = get_extension_properties(app, NULL, &ie_props, &eip_count);
  if (err) { wlu_freeup_vk(app); return; }

  for (uint32_t i = 0; i < eip_count; i++) {
    lower_to_upper(ie_props[i].extensionName);
    wlu_print_msg(WLU_INFO, "\t%d\t %s_EXTENSION_NAME\n", ie_props[i].specVersion, ie_props[i].extensionName);
  }

  wlu_print_msg(WLU_WARNING, "\n  Instance Extension Count: %d\n", eip_count);

  wlu_freeup_vk(app);
}

void print_device_extensions(VkPhysicalDeviceType dt) {
  VkResult err;
  vkcomp *app = wlu_init_vk();

  err = wlu_create_instance(app, "PrintStmt", "PrintStmt", 0, NULL, 3, instance_extensions);
  if (err) { wlu_freeup_vk(app); return; }

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props;
  VkPhysicalDeviceFeatures device_feats;
  err = wlu_create_physical_device(app, dt, &device_props, &device_feats);
  if (err) { wlu_freeup_vk(app); return; }

  wlu_print_msg(WLU_SUCCESS, "\n\t   Device Extension List\n  SpecVersion\t\tExtension Name\n\n");

  VkExtensionProperties *de_props = VK_NULL_HANDLE;
  uint32_t de_count = 0;

  err = get_extension_properties(app, NULL, &de_props, &de_count);
  if (err) { wlu_freeup_vk(app); return; }

  for (uint32_t i = 0; i < de_count; i++) {
    lower_to_upper(de_props[i].extensionName);
    wlu_print_msg(WLU_INFO, "\t%d\t %s_EXTENSION_NAME\n", de_props[i].specVersion, de_props[i].extensionName);
  }

  wlu_print_msg(WLU_WARNING, "\n  Device Extension Count: %d\n", de_count);

  wlu_freeup_vk(app);
}
