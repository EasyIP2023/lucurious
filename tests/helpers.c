
#include "lucom.h"

char *concat(char *fmt, ...) {
  char *result = NULL;
  int64_t size = 0;
  va_list args; /* type that holds variable arguments */

  /* Determine required size */
  va_start(args, fmt);
  size = vsnprintf(result,size,fmt,args);
  va_end(args);
  
  if (size < 0) return NULL;

  size++; /* for '/0' sentinal character */
  result = calloc(sizeof(result), size);
  if (!result) {
    wlu_log_me(WLU_DANGER, "[x] calloc: %s", strerror(errno));
    return NULL;
  }

  va_start(args, fmt);
  size = vsnprintf(result,size,fmt,args);
  va_end(args);

  // This disables compiler warnings
  if (!(size > 0)) {
    free(result);
    return NULL;
  }

  return result;
}