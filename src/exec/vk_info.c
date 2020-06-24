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

/* In helpers.c */
void lower_to_upper(char *s);

void print_validation_layers() {
  dlu_otma_mems ma = {.vkcomp_cnt = 1, .vkval_layer_cnt = 40 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) return;

  VkResult err;
  vkcomp *app = dlu_init_vk();
  if (!app) goto end;

  VkLayerProperties *vk_props = VK_NULL_HANDLE;
  uint32_t lcount = 0;
 
  err = dlu_set_vulkan_layer_props(&vk_props, &lcount);
  if (err) {
    dlu_freeup_vk(app);
    dlu_print_msg(DLU_DANGER, "[x] dlu_set_vulkan_layer_propsd failed\n");
    goto end_free_vk;
  }

  dlu_print_msg(DLU_SUCCESS, "\n\t\t\t\tValidation Layers List\n\t\tLayer Name\t\t\t\tDescription\n\n");

  for (uint32_t i = 0; i < lcount; i++) {
    dlu_print_msg(DLU_INFO, "\t%s", vk_props[i].layerName);
    dlu_print_msg(DLU_INFO, "\t\t%s\n", vk_props[i].description);
  }

  dlu_print_msg(DLU_WARNING, "\n\tValidation Layer Count: %d\n", lcount);

end_free_vk:
  dlu_freeup_vk(app);
end:
  dlu_release_blocks();
}

void print_instance_extensions() {
  dlu_otma_mems ma = { .vkcomp_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) return;

  VkResult err;
  vkcomp *app = dlu_init_vk();
  if (!app) goto end;

  err = dlu_create_instance(app, "Instance Extensions", "Instance Extensions", 0, NULL, 0, NULL);
  if (err) goto end_free_vk;

  dlu_print_msg(DLU_SUCCESS, "\n\t Instance Extension List\n  SpecVersion\t\tExtension Name\n\n");

  /* set available instance extensions */
  VkExtensionProperties *ie_props = VK_NULL_HANDLE;
  uint32_t eip_count = 0;

  err = get_extension_properties(NULL, &eip_count, &ie_props, NULL);
  if (err) goto end_free_vk;

  for (uint32_t i = 0; i < eip_count; i++) {
    lower_to_upper(ie_props[i].extensionName);
    dlu_print_msg(DLU_INFO, "\t%d\t %s_EXTENSION_NAME\n", ie_props[i].specVersion, ie_props[i].extensionName);
  }

  dlu_print_msg(DLU_WARNING, "\n  Instance Extension Count: %d\n", eip_count);

   free(ie_props);
end_free_vk:
  dlu_freeup_vk(app);
end:
  dlu_release_blocks();
}

void print_device_extensions(VkPhysicalDeviceType dt) {
  dlu_otma_mems ma = { .vkcomp_cnt = 1, .pd_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) return;

  VkResult err;
  vkcomp *app = dlu_init_vk();
  if (!app) goto end;

  err = dlu_otba(DLU_PD_DATA, app, INDEX_IGNORE, 1);
  if (!err) goto end_free_vk;

  err = dlu_create_instance(app, "Device Extensions", "Device Extensions", 0, NULL, 0,  NULL);
  if (err) goto end_free_vk;

  /*
  VkExtensionProperties *ie_props = VK_NULL_HANDLE;
  uint32_t eip_count = 0;

  err = get_extension_properties(NULL, &eip_count, &ie_props, NULL);
  if (err) goto end_free_vk;

  char **instance_extensions = alloca(eip_count * sizeof(char**));

  for (uint32_t i = 0; i < eip_count; i++)
    instance_extensions[i] = ie_props[i].extensionName;

  free(ie_props); ie_props = NULL;
  dlu_freeup_vk(app); app->instance = VK_NULL_HANDLE;
 
  for (uint32_t i = 0; i < eip_count; i++)
    dlu_print_msg(DLU_INFO, "%s\n", instance_extensions[i]);

  err = dlu_create_instance(app, "Device Extensions", "Device Extensions", 0, NULL, eip_count, (const char **) instance_extensions);
  if (err) goto end_free_vk;
  */

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props; VkPhysicalDeviceFeatures device_feats;
  err = dlu_create_physical_device(app, 0, dt, &device_props, &device_feats);
  if (err) { dlu_freeup_vk(app); return; }

  VkLayerProperties *lprops = VK_NULL_HANDLE; VkExtensionProperties *de_props = VK_NULL_HANDLE;
  uint32_t de_count = 0, lp_count = 0;

  err = get_layer_props(&lp_count, &lprops);
  if (err) goto end_free_vk;

  dlu_print_msg(DLU_SUCCESS, "\n\t   Device Extension List\n  SpecVersion\t\tExtension Name\n\n");

  for (uint32_t l = 0; l != lp_count; l++) {
    uint32_t count = 0;

    err = get_extension_properties(app->pd_data[0].phys_dev, &count, &de_props, (l == lp_count) ? lprops[l].layerName : NULL);
    if (err) goto end_free_lprops;
  
    for (uint32_t i = 0; i < count; i++)  {
      lower_to_upper(de_props[i].extensionName);
      dlu_print_msg(DLU_INFO, "\t%d\t %s_EXTENSION_NAME\n", de_props[i].specVersion, de_props[i].extensionName);
    }
  
    free(de_props); de_props = VK_NULL_HANDLE;
    de_count += count;
  }

  dlu_print_msg(DLU_WARNING, "\n  Device Extension Count: %d\n", de_count);

end_free_lprops:
  free(lprops);
end_free_vk:
  dlu_freeup_vk(app);
end:
  dlu_release_blocks();
}

void print_display_extensions(VkPhysicalDeviceType dt) {
  dlu_otma_mems ma = { .vkcomp_cnt = 1, .pd_cnt = 1, .dis_cnt = 1 };
  if (!dlu_otma(DLU_LARGE_BLOCK_PRIV, ma)) return;

  VkResult err;
  vkcomp *app = dlu_init_vk();
  if (!app) goto end;

  err = dlu_otba(DLU_PD_DATA, app, INDEX_IGNORE, 1);
  if (!err) goto end_free_vk;

  err = dlu_create_instance(app, "PrintStmt", "PrintStmt", 0, NULL, 0, NULL);
  if (err) goto end_free_vk;

  /* This will get the physical device, it's properties, and features */
  VkPhysicalDeviceProperties device_props; VkPhysicalDeviceFeatures device_feats;
  err = dlu_create_physical_device(app, 0, dt, &device_props, &device_feats);
  if (err) goto end_free_vk;

  err = dlu_get_physical_device_display_propertiesKHR(app, 0);
  if (err) goto end_free_vk;

  for (uint32_t i = 0; i < app->dpc; i++)
    dlu_print_msg(DLU_SUCCESS, "%s\n", app->dis_data[i].props.displayName);

end_free_vk:
  dlu_freeup_vk(app);
end:
  dlu_release_blocks();
}
