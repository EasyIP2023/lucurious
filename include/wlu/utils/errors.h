#ifndef WLU_ERRORS_H
#define WLU_ERRORS_H

int wlu_watch_me(
  int sig,
  int num_called,
  pid_t pid,
  void *app,
  void *wc
);

#endif
