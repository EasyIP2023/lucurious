#include <lucom.h>
#include <vlucur/vkall.h>

int main () {
  struct vkcomp app;
  VkResult err;

  initial_values(&app);

  err = check_validation_layer_support(&app);
  assert(!err);

  err = create_instance(&app, "Hello Triangle", "No Engine");
  assert(!err);

  cleanup(&app);

  return 0;
}
