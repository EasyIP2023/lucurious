/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#ifndef WLU_UTILS_LOG_H
#define WLU_UTILS_LOG_H

/* Used to help determine which ANSI Escape Codes to use */
typedef enum wlu_log_type {
  WLU_NONE    = 0x00000000,
  WLU_SUCCESS = 0x00000001,
  WLU_DANGER  = 0x00000002,
  WLU_INFO    = 0x00000003,
  WLU_WARNING = 0x00000004,
  WLU_RESET   = 0x00000005,
  WLU_MAX_LOG_ENUM = 0xFFFFFFFF
} wlu_log_type;

void _wlu_log_me(wlu_log_type type, FILE *stream, const char *fmt, ...);
const char *_wlu_strip_path(const char *filepath);

/* Macros defined to help better structure the message */
#define wlu_log_me(log_type, fmt, ...) \
  _wlu_log_me(log_type, stdout, "[%s:%d] " fmt, _wlu_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#define wlu_log_err(log_type, fmt, ...) \
  _wlu_log_me(log_type, stderr, "[%s:%d] " fmt, _wlu_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#endif
