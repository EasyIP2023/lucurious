/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#ifndef WLU_WAYLAND_TYPES_H
#define WLU_WAYLAND_TYPES_H

#include <linux/input-event-codes.h>

struct _wlu_way_core {
  struct wl_display *display;
  struct wl_compositor *compositor;
  struct wl_registry *registry;

  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;

  struct xdg_wm_base *shell;
  struct xdg_toplevel *xdg_toplevel;

  void *shm_data;
  struct wl_shm *shm;
  struct wl_buffer *buffer;

  uint32_t version;
  int running;
};

#ifdef LUCUR_WAYLAND_CLIENT_API
#include <wayland-client.h>
#include <wayland-client-protocol.h>
typedef struct _wlu_way_core wclient;
#endif

/* For now */
#ifdef LUCUR_WAYLAND_SERVER_API
#include <wayland-server.h>
#include <wayland-server-protocol.h>
typedef struct _wlu_way_core wserver;
#endif

#endif
