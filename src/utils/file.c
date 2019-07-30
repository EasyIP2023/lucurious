#include <lucom.h>
#include <errno.h>
#include <wlu/utils/file.h>
#include <wlu/utils/log.h>

wlu_file_info wlu_read_file(const char *filename) {
  struct wlu_file_info fileinfo = {NULL, 0};
  FILE *stream = NULL;

  /* Open the file in binary mode */
  stream = fopen(filename, "rb");
  if (!stream) {
    wlu_log_me(WLU_DANGER, "[x] %s: %s", filename, strerror(errno));
    return fileinfo;
  }

  /* Go to the end of the file */
  fileinfo.byte_size = fseek(stream, 0, SEEK_END);
  if (fileinfo.byte_size == -1) {
    wlu_log_me(WLU_DANGER, "[x] %s", strerror(errno));
    return fileinfo;
  }

  /* Get the current byte offset in the file */
  fileinfo.byte_size = ftell(stream);
  if (fileinfo.byte_size == -1) {
    wlu_log_me(WLU_DANGER, "[x] %s", strerror(errno));
    return fileinfo;
  }

  /* Jump back to the beginning of the file */
  rewind(stream);

  fileinfo.bytes = (char *) calloc(sizeof(char), (fileinfo.byte_size+1) * sizeof(char));
  if (!fileinfo.bytes) {
    wlu_log_me(WLU_DANGER, "[x] failed to calloc buff");
    return fileinfo;
  }

  /* Read in the entire file */
  fread(fileinfo.bytes, fileinfo.byte_size, 1, stream);
  fclose(stream);

  return fileinfo;
}
