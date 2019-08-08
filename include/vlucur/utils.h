#ifndef VLUCUR_UTILS_H
#define VLUCUR_UTILS_H

/* Can find in vulkan SDK API-Samples/utils/util.cpp */
bool memory_type_from_properties(
  vkcomp *app,
  uint32_t typeBits,
  VkFlags requirements_mask,
  uint32_t *typeIndex
);

#endif
