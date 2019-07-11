#include <lucom.h>
#include <utils/log.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server.h>

#define WLU_SRC_DIR "/home/vince/git/lucurious"

static const char *term_colors[] = {
	[WLU_NONE]   	= "",
	[WLU_SUCCESS] = "\x1B[32;1m",
	[WLU_DANGER ] = "\x1B[31;1m",
	[WLU_INFO   ] = "\x1B[30;1m",
	[WLU_WARNING] = "\x1B[33;1m",
	[WLU_RESET]   = "\x1b[0m",
};

void _wlu_log_me(wlu_log_type type, const char *fmt, ...) {
	char buffer[26];
  va_list args; /* type that holds variable arguments */

  /* create message stamp time */
	struct tm result;
	time_t t = time(NULL);
	struct tm *tm_info = localtime_r(&t, &result);

  /* generate time */
  strftime(buffer, sizeof(buffer), "%F %T - ", tm_info);
	fprintf(stdout, "%s", buffer);

	unsigned c = (type < WLU_MAX_LOG_ENUM) ? type : WLU_MAX_LOG_ENUM - 1;

	fprintf(stdout, "%s", term_colors[c]);

  /* Determine required size */
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);

	/* Reset terminal color */
	fprintf(stdout, "%s\n", term_colors[WLU_RESET]);
	fflush(stdout);
}

const char *_wlu_strip_path(const char *filepath) {
	static int srclen = sizeof(WLU_SRC_DIR);
	if (strstr(filepath, WLU_SRC_DIR) == filepath) {
		filepath += srclen;
	} else if (*filepath == '.') {
		while (*filepath == '.' || *filepath == '/') {
			++filepath;
		}
	}
	return filepath;
}
