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

#include "xdg-shell-protocol.h"

static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format) {
  ALL_UNUSED(data, wl_shm, format);
}

static struct wl_shm_listener shm_listener = {
	shm_format
};

void global_registry_handler(
  void *data,
  struct wl_registry *registry,
  uint32_t name,
  const char *interface,
  uint32_t version
) {
  struct _wlu_way_core *wc = (struct _wlu_way_core *) data;
  wc->version = version;

  // wlu_log_me(WLU_INFO, "Got a registry event for %s id %d", interface, name);
  if (!strcmp(interface, wl_compositor_interface.name)) {
    wc->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    wc->shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
  } else if (!strcmp(interface, wl_shm_interface.name)) {
    wc->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    wl_shm_add_listener(wc->shm, &shm_listener, NULL);
  }
}

void global_registry_remover(
  void *data,
  struct wl_registry *registry,
  uint32_t name
) {
  ALL_UNUSED(data, registry, name);
}
