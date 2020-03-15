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

#define LUCUR_WAYLAND_API
#define LUCUR_WAYLAND_CLIENT_API
#include <lucom.h>

#include "xdg-shell-client-protocol.h"

/**
* alloca()'s usage here is meant for stack space efficiency
* Fixed size arrays tend to over allocate, while alloca will
* allocate the exact amount of bytes that you want
*/

static void noop() {
  // This space intentionally left blank
}

static void xdg_surface_handle_configure(
  void *data,
  struct xdg_surface *xdg_surface,
  uint32_t serial
) {
  ALL_UNUSED(data);
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
  ALL_UNUSED(xdg_toplevel);
  wclient *wc = data;
  wc->running = 0;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = noop,
  .close = xdg_toplevel_handle_close,
};

static const struct wl_registry_listener registry_listener = {
  global_registry_handler,
  global_registry_remover
};

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

  err = wl_display_dispatch(wc->display);
  if (!err) return false;

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

  wl_surface_commit(wc->surface);

  return true;
}

bool wlu_create_wc_buffer(wclient *wc, uint32_t width, uint32_t height) {
  struct wl_shm_pool *pool = NULL;

  uint32_t stride = width * 4; /* a pixel is 4 bytes (RGBA) */
  uint32_t size = stride + height;
  int fd = NEG_ONE;

  fd = create_shm_file(size);
  if (fd < 0) return false;

  /**
  * asynchronously (set fd to non blocking mode into shm.c)
  * read contents from wayland shared file into buff
  */
  void *buff = alloca(size);
  if (read(fd, buff, size) == NEG_ONE) {
    wlu_log_me(WLU_DANGER, "[x] read: %s", strerror(errno));
    return false;
  }

  wc->shm_data = wlu_alloc(WLU_SMALL_BLOCK_SHARED, size);
  if (!wc->shm_data) { PERR(WLU_ALLOC_FAILED, 0, NULL); return false; }

  /* then memmove contents from the buff (which is a wayland shared file) into the block */
  wc->shm_data = memmove(wc->shm_data, buff, size);
  if (!wc->shm_data) {
    wlu_log_me(WLU_DANGER, "[x] memmove failed: Failed to copy file content");
    return false;
  }

  pool = wl_shm_create_pool(wc->shm, fd, size);
  wc->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);

  wl_shm_pool_destroy(pool);
  return true;
}
