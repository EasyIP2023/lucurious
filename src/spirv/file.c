/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2020 Vincent Davis Jr.
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

#define LUCUR_SPIRV_API
#include <lucom.h>

dlu_file_info dlu_read_file(const char *filename) {
  dlu_file_info fileinfo = {NULL, 0};
  FILE *stream = NULL;

  /* Open the file in binary mode */
  stream = fopen(filename, "rb");
  if (!stream) {
    dlu_log_me(DLU_DANGER, "[x] fopen: %s", filename, strerror(errno));
    goto exit_err;
  }

  /* Go to the end of the file */
  fileinfo.byte_size = fseek(stream, 0, SEEK_END);
  if (fileinfo.byte_size == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] fseek: %s", strerror(errno));
    goto exit_close;
  }

  /**
  * Get the current byte offset in the file.
  * Used read position to find the size of the
  * buffer we need to allocate
  */
  fileinfo.byte_size = ftell(stream);
  if (fileinfo.byte_size == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] ftell: %s", strerror(errno));
    goto exit_close;
  }

  /* Jump back to the beginning of the file */
  rewind(stream);

  fileinfo.bytes = (char *) calloc(fileinfo.byte_size, sizeof(char));
  if (!fileinfo.bytes) {
    dlu_log_me(DLU_DANGER, "[x] calloc: %s", strerror(errno));
    goto exit_close;
  }

  /* Read in the entire file */
  if (fread(fileinfo.bytes, fileinfo.byte_size, 1, stream) == 0) {
    dlu_log_me(DLU_DANGER, "[x] fread: %s", strerror(errno));
    goto exit_free;
  }

  fclose(stream);

  return fileinfo;
exit_free:
  free(fileinfo.bytes);
exit_close:
  fclose(stream);
exit_err:
  fileinfo.bytes = NULL;
  fileinfo.byte_size = 0;
  return fileinfo;
}
