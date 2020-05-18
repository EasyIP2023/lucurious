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

#include <lucom.h>

#include "client.h"
#include "xdg-shell-client-protocol.h"

static void noop() {
  // This space intentionally left blank
}

static void xdg_surface_handle_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {
  ALL_UNUSED(data);
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
  ALL_UNUSED(data, xdg_toplevel);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = noop,
  .close = xdg_toplevel_handle_close,
};

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t name) {
  ALL_UNUSED(data, registry, name);
}

static void global_registry_handler(
  void *data, struct wl_registry *registry,
  uint32_t name, const char *interface,
  uint32_t version
) {
  ALL_UNUSED(data, version);
  struct _wclient *wc = (struct _wclient *) data;

  wlu_log_me(WLU_INFO, "Got a registry event for %s id %d", interface, name);
  if (!strcmp(interface, wl_compositor_interface.name)) {
    wc->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    wc->shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
  }
}


static const struct wl_registry_listener registry_listener = {
  global_registry_handler,
  global_registry_remover
};

wclient *wlu_init_wc() {
  wclient *wc = calloc(1, sizeof(wclient));
  if (!wc) { wlu_log_me(WLU_DANGER, "[x] calloc: %s", strerror(errno)); return wc; }
  return wc;
}

void wlu_freeup_wc(wclient *wc) {
  if (wc->xdg_surface)
    xdg_surface_destroy(wc->xdg_surface);
  if (wc->surface)
    wl_surface_destroy(wc->surface);
  if (wc->shell)
    xdg_wm_base_destroy(wc->shell);
  if (wc->xdg_toplevel)
    xdg_toplevel_destroy(wc->xdg_toplevel);
  if (wc->registry)
    wl_registry_destroy(wc->registry);
  if (wc->compositor)
    wl_compositor_destroy(wc->compositor);
  if (wc->display)
    wl_display_disconnect(wc->display);
  free(wc);
}

bool wlu_create_client(wclient *wc) {
  int err = 0;

  /* establish connection to wayland server */
  wc->display = wl_display_connect(NULL);
  if (!wc->display) {
    wlu_log_me(WLU_DANGER, "[x] wl_diplay_connect: %s", strerror(errno));
    return false;
  }

  /* Registry gets server global information */
  wc->registry = wl_display_get_registry(wc->display);
  if (!wc->registry) return false;

  err = wl_registry_add_listener(wc->registry, &registry_listener, wc);
  if (err) return false;

  /* synchronously wait for the server respondes */
  err = wl_display_roundtrip(wc->display);
  if (!err) return false;

  if (!wc->compositor) {
    wlu_log_me(WLU_DANGER, "[x] Can't find compositor");
    return false;
  }

  if (!wc->shell) {
    wlu_log_me(WLU_DANGER, "[x] No xdg base support");
    return EXIT_FAILURE;
  }

  wc->surface = wl_compositor_create_surface(wc->compositor);
  if (!wc->surface) return false;

  wc->xdg_surface = xdg_wm_base_get_xdg_surface(wc->shell, wc->surface);
  if (!wc->xdg_surface) {
    wlu_log_me(WLU_DANGER, "[x] Can't create xdg_wm_base_get_xdg_surface");
    return false;
  }

  wc->xdg_toplevel = xdg_surface_get_toplevel(wc->xdg_surface);
  if (!wc->xdg_toplevel) {
    wlu_log_me(WLU_DANGER, "[x] Can't create xdg_surface_get_toplevel");
    return false;
  }

  err = xdg_surface_add_listener(wc->xdg_surface, &xdg_surface_listener, wc);
  if (err) return false;

  err = xdg_toplevel_add_listener(wc->xdg_toplevel, &xdg_toplevel_listener, wc);
  if (err) return false;

  wl_surface_commit(wc->surface);

  err = wl_display_roundtrip(wc->display);
  if (!err) return false;

  return true;
}
