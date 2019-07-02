#include <wlu/vkall.h>
#include <wlu/client.h>

int main() {
  VkResult err;
  wclient *wc = wlu_init_wc();
  vkcomp *app = wlu_init_vk();

  if (wlu_connect_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to connect client");
    return EXIT_FAILURE;
  }

  err = wlu_set_global_layers(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] checking and setting validation layers failed");
    return EXIT_FAILURE;
  }

  err = wlu_create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to create vulkan instance");
    return EXIT_FAILURE;
  }

  err = wlu_vkconnect_surfaceKHR(app, wc->display, wc->surface);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to connect to vulkan surfaceKHR");
    return EXIT_FAILURE;
  }

  err = wlu_enumerate_devices(app, VK_QUEUE_GRAPHICS_BIT, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to find physical device");
    return EXIT_FAILURE;
  }

  err = wlu_set_logical_device(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to initialize logical device to physical device");
    return EXIT_FAILURE;
  }

  err = wlu_create_swap_chain(app);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to create swap chain");
    return EXIT_FAILURE;
  }

  err = wlu_create_img_views(app, TWO_D_IMG);
  if (err) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to create image views");
    return EXIT_FAILURE;
  }

  if (wlu_run_client(wc)) {
    wlu_freeup_wc(wc);
    wlu_freeup_vk(app);
    fprintf(stderr, "[x] failed to run wayland client");
    return EXIT_FAILURE;
  }

  wlu_freeup_wc(wc);
  wlu_freeup_vk(app);

  return EXIT_SUCCESS;
}
