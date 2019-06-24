#ifndef CLIENT_H
#define CLIENT_H

#include <wayland-client.h>
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

struct wclient *wlu_init_wc();
int wlu_connect_client(struct wclient *wc);
int wlu_run_client(struct wclient *wc);
void wlu_freeup_wc(void *data);

#endif
