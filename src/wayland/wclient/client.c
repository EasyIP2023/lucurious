#include <sys/mman.h>
#include <unistd.h>

#include <wclient/client.h>
#include <wclient/shm.h>

#include "xdg-shell-client-protocol.h"

static void noop() {
  // This space intentionally left blank
}

static void xdg_surface_handle_configure(void *data,
    struct xdg_surface *xdg_surface, uint32_t serial) {
  ALL_UNUSED(data);
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
  ALL_UNUSED(xdg_toplevel);
  struct wclient *wc = data;
  wc->running = 0;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = noop,
  .close = xdg_toplevel_handle_close,
};

static void pointer_handle_button(void *data, struct wl_pointer *pointer,
		uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
  struct wclient *wc = data;
  ALL_UNUSED(time, pointer);
	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
		xdg_toplevel_move(wc->xdg_toplevel, wc->seat, serial);
	}
}

static const struct wl_pointer_listener pointer_listener = {
	.enter = noop,
	.leave = noop,
	.motion = noop,
	.button = pointer_handle_button,
	.axis = noop,
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat,
		uint32_t capabilities) {
  ALL_UNUSED(data);
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, seat);
	}
}

static const struct wl_seat_listener seat_listener = {
	.capabilities = seat_handle_capabilities,
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t name,
	  const char *interface, uint32_t version) {
  fprintf(stdout, "Got a registry event for %s id %d\n", interface, name);
  struct wclient *wc = data;
  wc->version = version;
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    wc->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    wc->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    wc->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    wc->seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    wl_seat_add_listener(wc->seat, &seat_listener, NULL);
  }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t name) {
  ALL_UNUSED(data, registry);
  printf("Got a registry losing event for %d\n", name);
}

static const struct wl_registry_listener registry_listener = {
  global_registry_handler,
  global_registry_remover
};

static struct wl_buffer *create_buffer(struct wclient *wc) {
	int stride = 1024 * 4;
	int size = stride * 681;

	int fd = create_shm_file(size);
	if (fd < 0) {
		fprintf(stderr, "[x] creating a buffer file for %d B failed: %m\n", size);
		return NULL;
	}

	wc->shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (wc->shm_data == MAP_FAILED) {
		fprintf(stderr, "[x] mmap failed: %m\n");
		close(fd);
		return NULL;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(wc->shm, fd, size);
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, 1024, 681,
		stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);

	// MagickImage is from waves.h
	// memcpy(wc->shm_data, MagickImage, size);
	return buffer;
}

void initialize_wclient_values(struct wclient *wc) {
  wc->display = NULL;
  wc->registry = NULL;
  wc->buffer = NULL;
  wc->surface = NULL;
  wc->xdg_surface = NULL;
  wc->compositor = NULL;
  wc->seat = NULL;
  wc->shm = NULL;
  wc->xdg_wm_base = NULL;
  wc->xdg_toplevel = NULL;
  wc->shm_data = NULL;
  wc->running = 1;
}

void connect_client(struct wclient *wc) {
  wc->display = wl_display_connect(NULL);
  assert(wc->display != NULL);

  printf("connected to display\n");

  wc->registry = wl_display_get_registry(wc->display);
  wl_registry_add_listener(wc->registry, &registry_listener, wc);

  wl_display_dispatch(wc->display);
  wl_display_roundtrip(wc->display);

  if (wc->compositor == NULL) {
    fprintf(stderr, "[x] Can't find compositor\n");
    return;
  } else {
    fprintf(stdout, "Found compositor\n");
  }

  if (wc->xdg_wm_base == NULL) {
    fprintf(stderr, "[x] No xdg_wm_base support\n");
    return;
  }

  wc->buffer = create_buffer(wc);
  if (wc->buffer == NULL) return;

  wc->surface = wl_compositor_create_surface(wc->compositor);
  wc->xdg_surface = xdg_wm_base_get_xdg_surface(wc->xdg_wm_base, wc->surface);

  if (wc->xdg_surface == NULL) {
    fprintf(stderr, "[x] Can't create xdg_shell_surface\n");
    return;
  } else {
    fprintf(stdout, "Created xdg_shell_surface\n");
  }

  wc->xdg_toplevel = xdg_surface_get_toplevel(wc->xdg_surface);
  xdg_surface_add_listener(wc->xdg_surface, &xdg_surface_listener, wc);
  xdg_toplevel_add_listener(wc->xdg_toplevel, &xdg_toplevel_listener, wc);
  printf("Add xdg listeners\n");

  wl_surface_commit(wc->surface);
  wl_display_roundtrip(wc->display);

  wl_surface_attach(wc->surface, wc->buffer, 0, 0);
  wl_surface_commit(wc->surface);
}

int run_client(struct wclient *wc) {
  while (wl_display_dispatch(wc->display) != -1 && wc->running) {
    // This space intentionally left blank
  }
  return EXIT_SUCCESS;
}

VkWaylandSurfaceCreateInfoKHR set_wayland_surface_ciKHR(struct wclient *wc) {

  VkWaylandSurfaceCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.display = wc->display;
  create_info.surface = wc->surface;

  return create_info;
}

void freeup_wc(struct wclient *wc) {
  xdg_toplevel_destroy(wc->xdg_toplevel);
  xdg_surface_destroy(wc->xdg_surface);
  wl_surface_destroy(wc->surface);
  wl_seat_destroy(wc->seat);
  wl_compositor_destroy(wc->compositor);
  wl_registry_destroy(wc->registry);
  wl_display_disconnect(wc->display);
  initialize_wclient_values(wc);
}
