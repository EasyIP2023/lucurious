#include <lucom.h>
#include <vlucur/vkall.h>
#include <wclient/client.h>

int main () {
  VkResult err;
  struct vkcomp *app = init_vk();
  struct wclient *wc = init_wc();

  if (connect_client(wc)) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to connect client");
    return EXIT_FAILURE;
  }

  err = set_global_layers(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] checking for validation layer support failed");
    return EXIT_FAILURE;
  }

  err = create_instance(app, "Hello Triangle", "No Engine");
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to create vulkan instance");
    return EXIT_FAILURE;
  }

  err = vk_connect_surfaceKHR(app, get_display(wc), get_surface(wc));
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to connect to vulkan surfaceKHR");
    return EXIT_FAILURE;
  }

  err = enumerate_devices(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to find physical device");
    return EXIT_FAILURE;
  }

  err = set_logical_device(app);
  if (err) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to initialize logical device to physical device");
    return EXIT_FAILURE;
  }

  if (run_client(wc)) {
    freeup_wc(wc);
    freeup_vk(app);
    fprintf(stderr, "[x] failed to run wayland client");
    return EXIT_FAILURE;
  }

  freeup_wc(wc);
  freeup_vk(app);

  return EXIT_SUCCESS;
}
