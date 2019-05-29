#include <wclient/client.h>
#include <wclient/waves.h>
#include <wclient/shm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "xdg-shell-client-protocol.h"

/* Redirecting ouput of a few variable to /dev/null for now */
#define DEBUG

#ifdef DEBUG
  FILE *debug = NULL;
#else
  FILE *debug = stderr; /* or stdout */
#endif

struct wl_compositor *compositor = NULL;
struct wl_seat *seat = NULL;

static struct wl_shm *shm = NULL;
static struct xdg_wm_base *xdg_wm_base = NULL;
static struct xdg_toplevel *xdg_toplevel = NULL;

static const int width = 1024;
static const int height = 681;

static void *shm_data = NULL;

static int running = 1;

static void noop() {
  // This space intentionally left blank
}

static void xdg_surface_handle_configure(void *data,
    struct xdg_surface *xdg_surface, uint32_t serial) {
  fprintf(debug,"%p\n", data);
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_close(void *data,
    struct xdg_toplevel *xdg_toplevel) {
  fprintf(debug,"%p : %p\n", data, xdg_toplevel);
  running = 0;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = noop,
  .close = xdg_toplevel_handle_close,
};

static void pointer_handle_button(void *data, struct wl_pointer *pointer,
		uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
	struct wl_seat *seat = data;
	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
		xdg_toplevel_move(xdg_toplevel, seat, serial);
	}
  fprintf(debug, "%p : %d : %p\n", data, time, pointer);
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
	if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
		struct wl_pointer *pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(pointer, &pointer_listener, seat);
	}
  fprintf(debug, "%p\n", data);
}

static const struct wl_seat_listener seat_listener = {
	.capabilities = seat_handle_capabilities,
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t name,
	  const char *interface, uint32_t version) {
  printf("Got a registry event for %s id %d\n", interface, name);
  fprintf(debug, "%p : %d\n", data, version);
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    wl_seat_add_listener(seat, &seat_listener, NULL);
  }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t name) {
  printf("Got a registry losing event for %d\n", name);
  fprintf(debug, "%p : %p\n", data, registry);
}

static const struct wl_registry_listener registry_listener = {
  global_registry_handler,
  global_registry_remover
};

static struct wl_buffer *create_buffer() {
	int stride = width * 4;
	int size = stride * height;

	int fd = create_shm_file(size);
	if (fd < 0) {
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
		return NULL;
	}

	shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_data == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		return NULL;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height,
		stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);

	// MagickImage is from waves.h
	memcpy(shm_data, MagickImage, size);
	return buffer;
}

int runme(void) {

  /* Redirecting ouput of a few variable to /dev/null for now */
  debug = fopen("/dev/null", "w");

  struct wl_display *display = wl_display_connect(NULL);
  if (display == NULL) {
    fprintf(stderr, "Can't connect to display\n");
    return EXIT_FAILURE;
  }
  printf("connected to display\n");

  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, NULL);

  wl_display_dispatch(display);
  wl_display_roundtrip(display);

  if (compositor == NULL) {
    fprintf(stderr, "Can't find compositor\n");
    return EXIT_FAILURE;
  } else {
    fprintf(stderr, "Found compositor\n");
  }

  if (xdg_wm_base == NULL) {
    fprintf(stderr, "[x] No xdg_wm_base support\n");
    goto free;
  }

  struct wl_buffer *buffer = create_buffer();
  if (buffer == NULL) {
    return EXIT_FAILURE;
  }

  struct wl_surface *surface = wl_compositor_create_surface(compositor);
  struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);

  if (xdg_surface == NULL) {
    fprintf(stderr, "Can't create xdg_shell_surface\n");
    return EXIT_FAILURE;
  } else {
    fprintf(stderr, "Created xdg_shell_surface\n");
  }
  xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
  xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);
  xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, NULL);
  printf("Add xdg listeners\n");

  wl_surface_commit(surface);
  wl_display_roundtrip(display);

  wl_surface_attach(surface, buffer, 0, 0);
  wl_surface_commit(surface);

  while (wl_display_dispatch(display) != -1 && running) {
    // This space intentionally left blank
  }

  xdg_toplevel_destroy(xdg_toplevel);
  xdg_surface_destroy(xdg_surface);
  wl_surface_destroy(surface);
  printf("Destroy xdg_toplevel, xdg_surface, & wl_surface\n");
free:
  wl_display_disconnect(display);
  printf("disconnected from display\n");

  return EXIT_SUCCESS;
}
