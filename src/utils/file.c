#include <lucom.h>
#include <errno.h>
#include <utils/file.h>
#include <wlu/utils/log.h>

const char *wlu_read_file(const char *filename) {
  FILE *stream = NULL;
  char *buff = NULL;
  long filelen;

  /* Open the file in binary mode */
  stream = fopen(filename, "rb");
  if (!stream) {
    wlu_log_me(WLU_DANGER, "[x] %s: %s", filename, strerror(errno));
    return NULL;
  }

  /* Go to the end of the file */
  filelen = fseek(stream, 0, SEEK_END);
  if (filelen == -1) {
    wlu_log_me(WLU_DANGER, "[x] %s", strerror(errno));
    return NULL;
  }

  /* Get the current byte offset in the file */
  filelen = ftell(stream);
  if (filelen == -1) {
    wlu_log_me(WLU_DANGER, "[x] %s", strerror(errno));
    return NULL;
  }

  /* Jump back to the beginning of the file */
  rewind(stream);

  buff = (char *) calloc(sizeof(char), (filelen+1) * sizeof(char));
  if (!buff) {
    wlu_log_me(WLU_DANGER, "[x] failed to calloc buff");
    return NULL;
  }

  /* Read in the entire file */
  fread(buff, filelen, 1, stream);
  fclose(stream);

  return buff;
}
