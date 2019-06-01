#include <vlucur/vkall.h>

void run_vulkan_test() {
  vkcomp *app = create_app(1);
  reset_values(app);
  init_vulkan(app);
  cleanup(app);
}
