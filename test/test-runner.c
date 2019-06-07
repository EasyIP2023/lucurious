#include <vlucur/vkall.h>
#include <wclient/client.h>

int main () {

  wclient *wc = create_client(1);
  connect_client(wc);

  vkcomp *app = create_app(1);
  reset_values(app);
  create_instance(app);
  setup_debug_messenger(app);
  pick_graphics_device(app);
  create_logical_device(app);
  create_surface(app, get_wl_display(wc), get_wl_surface(wc));

  run_client(wc);
  free_wclient(wc);
  cleanup(app);

  return 0;
}
