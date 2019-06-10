#include <lucom.h>
#include <vlucur/vkall.h>
#include <wclient/client.h>

int main () {
  VkResult err;
  struct vkcomp app;
  struct wclient wc;

  initialize_vulkan_values(&app);
  initialize_wclient_values(&wc);

  connect_client(&wc);

  err = check_validation_layer_support(&app);
  assert(!err);

  err = create_instance(&app, "Hello Triangle", "No Engine");
  assert(!err);

  err = enumerate_devices(&app);
  assert(!err);

  err = init_logical_device(&app);
  assert(!err);

  err = vk_connect_surfaceKHR(&app, set_wayland_surface_ciKHR(&wc));
  //assert(!err);

  run_client(&wc);

  freeup_wc(&wc);
  freeup_vk(&app);

  return 0;
}
