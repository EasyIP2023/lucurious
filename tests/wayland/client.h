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

#ifndef DLU_WAYLAND_CLIENT_H
#define DLU_WAYLAND_CLIENT_H

#include <wayland-client.h>
#include <wayland-client-protocol.h>

typedef struct _wclient {
  struct wl_display *display;
  struct wl_compositor *compositor;
  struct wl_registry *registry;

  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;

  struct xdg_wm_base *shell;
  struct xdg_toplevel *xdg_toplevel;
} wclient;

wclient *dlu_init_wc();
void dlu_freeup_wc(wclient *wc);
bool dlu_create_client(wclient *wc);

#endif
