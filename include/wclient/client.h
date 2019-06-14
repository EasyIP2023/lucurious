#ifndef CLIENT_H
#define CLIENT_H

#include <lucom.h>

#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>

struct wclient {
  struct wl_compositor *compositor;
  struct wl_seat *seat;

  struct wl_shm *shm;
  struct xdg_wm_base *xdg_wm_base;
  struct xdg_toplevel *xdg_toplevel;

  void *shm_data;

  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_buffer *buffer;

  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;

  uint32_t version;
  int running;
};

struct wclient *init_wc();
void connect_client(struct wclient *wc);
int run_client(struct wclient *wc);
VkWaylandSurfaceCreateInfoKHR set_wayland_surface_ciKHR(struct wclient *wc);
void freeup_wc(void *data);

#endif
