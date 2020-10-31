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

#include <lucom.h>
#include <time.h>

/* ANSI Escape Codes */
static const char *term_colors[] = {
  [DLU_NONE]    = "",
  [DLU_SUCCESS] = "\e[32;1m",
  [DLU_DANGER]  = "\e[31;1m",
  [DLU_INFO]    = "\e[37;1m",
  [DLU_WARNING] = "\e[33;1m",
  [DLU_RESET]   = "\x1b[0m"
};

void _dlu_log_me(dlu_log_type type, FILE *stream, const char *fmt, ...) {
  char buffer[26];
  va_list args; /* type that holds variable arguments */

  /* create message time stamp */
  time_t rawtime = time(NULL);

  /* generate time */
  strftime(buffer, sizeof(buffer), "%F %T - ", localtime_r(&rawtime, &(struct tm){}));
  fprintf(stream, "%s", buffer);

  /* Set terminal color */
  fprintf(stream, term_colors[type]);
  va_start(args, fmt);
  vfprintf(stream, fmt, args);
  va_end(args); /* Reset terminal color */
  fprintf(stream, term_colors[DLU_RESET]);

  /* Flush twice */
  fprintf(stream, "\n");
  fflush(stream);
}

void _dlu_print_me(dlu_log_type type, const char *msg, ...) {
  va_list args;

  /* Set terminal color */
  fprintf(stdout, term_colors[type]);
  va_start(args, msg);
  vfprintf(stdout, msg, args);
  va_end(args); /* Reset terminal color */
  fprintf(stdout, term_colors[DLU_RESET]);

  fflush(stdout);
}

/* Modified version of what was in wlroots/util/log.c */
const char *_dlu_strip_path(const char *filepath) {
  if (*filepath == '.')
    while (*filepath == '.' || *filepath == '/')
      filepath++;
  return filepath;
}
