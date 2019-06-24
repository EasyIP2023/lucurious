#include <lucom.h>
#include <sys/mman.h>

#include <wclient/client.h>
#include <wclient/shm.h>

#include "xdg-shell-client-protocol.h"

static void set_values(struct wclient *wc) {
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

struct wclient *wlu_init_wc() {
  struct wclient *wc = calloc(sizeof(struct wclient), sizeof(struct wclient));
  assert(wc != NULL);
  set_values(wc);
  return wc;
}

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

/* static void pointer_handle_button(void *data, struct wl_pointer *pointer,
		uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
  struct wclient *wc = (struct wclient *) data;
  ALL_UNUSED(time, pointer);
	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
		xdg_toplevel_move(wc->xdg_toplevel, wc->seat, serial);
	}
} */

static const struct wl_pointer_listener pointer_listener = {
	.enter = noop,
	.leave = noop,
	.motion = noop,
	.button = noop,
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
  struct wclient *wc = (struct wclient *) data;
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

int wlu_connect_client(struct wclient *wc) {
  int err = 0;

  wc->display = wl_display_connect(NULL);
  if (!wc->display) return EXIT_FAILURE;

  printf("connected to display\n");

  wc->registry = wl_display_get_registry(wc->display);
  if (!wc->registry) return EXIT_FAILURE;

  err = wl_registry_add_listener(wc->registry, &registry_listener, wc);
  if (err) return EXIT_FAILURE;

  err = wl_display_dispatch(wc->display);
  if (!err) return EXIT_FAILURE;

  err = wl_display_roundtrip(wc->display);
  if (!err) return EXIT_FAILURE;

  if (!wc->compositor) {
    fprintf(stderr, "[x] Can't find compositor\n");
    return EXIT_FAILURE;
  } else {
    fprintf(stdout, "Found compositor\n");
  }

  if (!wc->xdg_wm_base) {
    fprintf(stderr, "[x] No xdg_wm_base support\n");
    return EXIT_FAILURE;
  }

  wc->buffer = create_buffer(wc);
  if (!wc->buffer) return EXIT_FAILURE;

  wc->surface = wl_compositor_create_surface(wc->compositor);
  if (!wc->surface) return EXIT_FAILURE;

  wc->xdg_surface = xdg_wm_base_get_xdg_surface(wc->xdg_wm_base, wc->surface);
  if (!wc->xdg_surface) return EXIT_FAILURE;

  if (wc->xdg_surface == NULL) {
    fprintf(stderr, "[x] Can't create xdg_shell_surface\n");
    return EXIT_FAILURE;
  } else {
    fprintf(stdout, "Created xdg_shell_surface\n");
  }

  wc->xdg_toplevel = xdg_surface_get_toplevel(wc->xdg_surface);
  if (!wc->xdg_toplevel) return EXIT_FAILURE;

  err = xdg_surface_add_listener(wc->xdg_surface, &xdg_surface_listener, wc);
  if (err) return EXIT_FAILURE;

  err = xdg_toplevel_add_listener(wc->xdg_toplevel, &xdg_toplevel_listener, wc);
  if (err) return EXIT_FAILURE;

  printf("Add xdg listeners\n");

  wl_surface_commit(wc->surface);

  err = wl_display_roundtrip(wc->display);
  if (!err) return EXIT_FAILURE;

  wl_surface_attach(wc->surface, wc->buffer, 0, 0);
  wl_surface_commit(wc->surface);

  return err = 0;
}

int wlu_run_client(struct wclient *wc) {
  while (wl_display_dispatch(wc->display) != -1 && wc->running) {
    // This space intentionally left blank
  }
  return EXIT_SUCCESS;
}

void wlu_freeup_wc(void *data) {
  struct wclient *wc = (struct wclient*) data;

  if (wc->xdg_toplevel)
    xdg_toplevel_destroy(wc->xdg_toplevel);
  if (wc->xdg_surface)
    xdg_surface_destroy(wc->xdg_surface);
  if (wc->surface)
    wl_surface_destroy(wc->surface);
  if (wc->buffer)
    wl_buffer_destroy(wc->buffer);
  if (wc->shm)
    wl_shm_destroy(wc->shm);
  if (wc->seat)
    wl_seat_destroy(wc->seat);
  if (wc->compositor)
    wl_compositor_destroy(wc->compositor);
  if (wc->registry)
    wl_registry_destroy(wc->registry);
  if (wc->display)
    wl_display_disconnect(wc->display);
  if (wc->shm_data) {
    int size = 1024 * 4 * 681;
    munmap(wc->shm_data, size);
  }

  // wc->xdg_wm_base = NULL; Not sure about this one
  if (wc->xdg_wm_base)
    free(wc->xdg_wm_base);

  set_values(wc);
  if (wc)
    free(wc);
  wc = NULL;
}
