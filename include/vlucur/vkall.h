#ifndef VK_ALL
#define Vk_ALL

#include <stdlib.h>

typedef struct vkcomp vkcomp;

/* Function protypes */
vkcomp *create_app(size_t init_value);
void reset_values(vkcomp *app);
void init_vulkan(vkcomp *app);
void cleanup(vkcomp *app);

#endif
