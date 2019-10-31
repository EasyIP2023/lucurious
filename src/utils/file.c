/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 EasyIP2023
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

  /*
   * Get the current byte offset in the file.
   * Used read position to find the size of the
   * buffer we need to allocate
   */
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
