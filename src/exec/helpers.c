#include <lucom.h>
#include <vlucur/vkall.h>
#include <exec/helpers.h>

/* https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c */
char **str_split(char *a_str, const char a_delim) {
  char  **result   = NULL;
  size_t count     = 0;
  char *tmp        = a_str;
  char *last_comma = NULL;
  char delim[2];

  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp){
    if (a_delim == *tmp) {
      count++;
      last_comma = tmp;
    }
    tmp++;
  }

  /* Just encase user supplies wrong number of arguments */
  if (count != 1)
    return NULL;

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = calloc(sizeof(char*), count * sizeof(char*));

  if (result) {
    size_t idx  = 0;
    char *token = strtok(a_str, delim);

    while (token) {
      if (idx > count) {
        free(result);
        result = NULL;
        return result;
      }
      *(result + idx++) = strdup(token);
      token = strtok(0, delim);
    }
    if (idx != (count - 1)) {
      free(result);
      result = NULL;
      return result;
    }
    *(result + idx) = 0;
  }

  return result;
}

VkQueueFlagBits ret_qfambit(char *str) {
  if (!strcmp(str, "VK_QUEUE_GRAPHICS_BIT")) return VK_QUEUE_GRAPHICS_BIT;
  if (!strcmp(str, "VK_QUEUE_COMPUTE_BIT")) return VK_QUEUE_COMPUTE_BIT;
  if (!strcmp(str, "VK_QUEUE_TRANSFER_BIT")) return VK_QUEUE_TRANSFER_BIT;
  if (!strcmp(str, "VK_QUEUE_SPARSE_BINDING_BIT")) return VK_QUEUE_SPARSE_BINDING_BIT;
  if (!strcmp(str, "VK_QUEUE_PROTECTED_BIT")) return VK_QUEUE_PROTECTED_BIT;

  fprintf(stdout, "[x] No correct option was selected returning VK_QUEUE_FLAG_BITS_MAX_ENUM\n");
  fprintf(stdout, "[x] For a list of available flags go here\n");
  fprintf(stdout, "[x] https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkQueueFlagBits.html\n\n");

  return VK_QUEUE_FLAG_BITS_MAX_ENUM;
}

VkPhysicalDeviceType ret_dtype(char *str) {
  if (!strcmp(str, "0")) return VK_PHYSICAL_DEVICE_TYPE_OTHER;
  if (!strcmp(str, "1")) return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
  if (!strcmp(str, "2")) return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
  if (!strcmp(str, "3")) return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
  if (!strcmp(str, "4")) return VK_PHYSICAL_DEVICE_TYPE_CPU;

  fprintf(stdout, "[x] No correct option was selected returning VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM\n");
  fprintf(stdout, "[x] For a list of available numbers go here, usage: [0-4]\n");
  fprintf(stdout, "[x] https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceType.html\n\n");

  return VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
}