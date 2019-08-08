#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <vlucur/utils.h>

/* Can find in vulkan SDK API-Samples/utils/util.cpp */
bool memory_type_from_properties(vkcomp *app, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {

  vkGetPhysicalDeviceMemoryProperties(app->physical_device, &app->memory_properties);

  /* Search memtypes to find first index with those properties */
  for (uint32_t i = 0; i < app->memory_properties.memoryTypeCount; i++) {
    if ((typeBits & 1) == 1) {
      /* Type is available, does it match user properties */
      if ((app->memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  /* No memory types matched, return failure */
  return false;
}
