/*
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

#include <wlu/utils/log.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

static const char *term_colors[] = {
	[WLU_NONE]   	= "",
	[WLU_SUCCESS] = "\x1B[32;1m",
	[WLU_DANGER] 	= "\x1B[31;1m",
	[WLU_INFO] 		= "\x1B[30;1m",
	[WLU_WARNING] = "\x1B[33;1m",
	[WLU_RESET]   = "\x1b[0m",
};

void _wlu_log_me(wlu_log_type type, FILE *stream, const char *fmt, ...) {
	char buffer[26];
	va_list args; /* type that holds variable arguments */

	/* create message time stamp */
	struct tm result;
	time_t t = time(NULL);
	struct tm *tm_info = localtime_r(&t, &result);

	/* generate time */
	strftime(buffer, sizeof(buffer), "%F %T - ", tm_info);
	fprintf(stream, "%s", buffer);

	unsigned c = (type < WLU_MAX_LOG_ENUM) ? type : WLU_MAX_LOG_ENUM - 1;

	fprintf(stream, "%s", term_colors[c]);

	va_start(args, fmt);
	vfprintf(stream, fmt, args);
	va_end(args);

	/* Reset terminal color */
	fprintf(stream, "%s\n", term_colors[WLU_RESET]);
}

/* Modified version of what's in wlroots */
const char *_wlu_strip_path(const char *filepath) {
	if (*filepath == '.')
		while (*filepath == '.' || *filepath == '/')
			filepath++;
	return filepath;
}
