#include <lucom.h>
#include <vlucur/vkall.h>

int main () {
  VkResult err;
  struct vkcomp app;

  initial_values(&app);
  err = check_validation_layer_support(&app);
  assert(!err);
  err = create_instance(&app, "Hello Triangle", "No Engine");
  assert(!err);
  err = enumerate_devices(&app);
  assert(!err);
  err = init_logical_device(&app);
  assert(!err);
  cleanup(&app);

  return 0;
}
