#include <stdlib.h>

#ifndef VKALL_H
#define VKALL_H

typedef struct vkcomp vkcomp;

/* Function protypes */
vkcomp *create_app(size_t init_value);
void reset_values(vkcomp *app);
void create_instance(vkcomp *app);
void setup_debug_messenger(vkcomp *app);
void create_surface(vkcomp *app, void *wl_display, void *wl_surface);
void pick_graphics_device(vkcomp *app);
void create_logical_device(vkcomp *app);
void cleanup(vkcomp *app);

#endif
