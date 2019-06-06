#ifndef CLIENT_H
#define CLIENT_H

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <stdlib.h>

/* http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/ */
/* https://stackoverflow.com/questions/23235910/variadic-unused-function-macro */
#define UNUSED1(z) (void)(z)
#define UNUSED2(y,z) UNUSED1(y),UNUSED1(z)
#define UNUSED3(x,y,z) UNUSED1(x),UNUSED2(y,z)
#define UNUSED4(b,x,y,z) UNUSED2(b,x),UNUSED2(y,z)
#define UNUSED5(a,b,x,y,z) UNUSED2(a,b),UNUSED3(x,y,z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL( VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__ )

typedef struct wclient wclient;

wclient *create_client(size_t init_value);
void free_wclient(wclient *wc);
int run_client(wclient *wc);
void connect_client(wclient *wc);
struct wl_display *get_wl_display(wclient *wc);
struct wl_surface *get_wl_surface(wclient *wc);

#endif
