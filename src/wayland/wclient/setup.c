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

#define LUCUR_WAYLAND_API
#define LUCUR_WAYLAND_CLIENT_API
#include <lucom.h>

#include "xdg-shell-client-protocol.h"

bool wlu_run_client(wclient *wc) {
  if (wl_display_dispatch(wc->display) != NEG_ONE && wc->running)
    return false;
  return true;
}

wclient *wlu_init_wc() {
  wclient *wc = wlu_alloc(WLU_SMALL_BLOCK_PRIV, sizeof(wclient));
  if (!wc) { PERR(WLU_ALLOC_FAILED, 0, NULL); return wc; }

  wc->running = 1;
  return wc;
}

void wlu_freeup_wc(void *data) {
  wclient *wc = (wclient *) data;

  if (wc->shm)
    wl_shm_destroy(wc->shm);
  if (wc->buffer)
    wl_buffer_destroy(wc->buffer);
  if (wc->surface)
    wl_surface_destroy(wc->surface);
  if (wc->xdg_surface)
    xdg_surface_destroy(wc->xdg_surface);
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
}
