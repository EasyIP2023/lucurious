#ifndef WLU_LOG_H
#define WLU_LOG_H

typedef enum wlu_log_type {
  WLU_NONE = 0,
  WLU_SUCCESS = 1,
  WLU_DANGER = 2,
  WLU_INFO = 3,
  WLU_WARNING = 4,
  WLU_RESET = 5,
  WLU_MAX_LOG_ENUM
} wlu_log_type;

void _wlu_log_me(wlu_log_type type, const char *fmt, ...);
const char *_wlu_strip_path(const char *filepath);

#define wlu_log_me(log_type, fmt, ...) \
  _wlu_log_me(log_type, "[%s:%d] " fmt, _wlu_strip_path(__FILE__), __LINE__, ##__VA_ARGS__)

#endif