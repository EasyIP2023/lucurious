/* https://github.com/emersion/hello-wayland/blob/master/shm.h */
#ifndef WLU_WAYLAND_UTILS_SHM_H
#define WLU_WAYLAND_UTILS_SHM_H

#include <sys/types.h>

int create_shm_file(off_t size);

#endif
