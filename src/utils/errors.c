#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/wclient/client.h>
#include <wlu/utils/log.h>
#include <wlu/utils/errors.h>
#include <signal.h>

/*
 * global struct used to copy data over
 * and pass it into the signal handler
 * I'm assuming that the user called the signal
 * handler each time a new process is created
 */
struct wlu_sig_info {
  pid_t pid;
  vkcomp *gapp;
  wclient *gwc;
} wsi;

static void signal_handler(int sig, siginfo_t *info, void *extra) {
  ALL_UNUSED(extra);

  wlu_log_me(WLU_DANGER, "[x] Sigfault bruh... Fix it :(");
  wlu_log_me(WLU_DANGER, "[x] Process ID: %d | Received signal: %i (Segmentation fault) | Signal Code: %d",
            wsi.pid, sig, info->si_code);
  wlu_log_me(WLU_DANGER, "[x] Errno Value: %d | Exit Value Status: %d",
            info->si_errno, info->si_status);
  wlu_log_me(WLU_DANGER, "[x] Address of segfualt: %p", info->si_addr);
  wlu_log_me(WLU_DANGER, "[x] Caught and freed up memory for");
  wlu_log_me(WLU_DANGER, "[x] vkcomp struct: %p", wsi.gapp);

  wlu_freeup_vk(wsi.gapp);
  wlu_freeup_wc(wsi.gwc);

  exit(EXIT_FAILURE);
}

int wlu_catch_me(pid_t pid, vkcomp *app, wclient *wc) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(sigaction));
  sigemptyset(&sa.sa_mask);

  memcpy(&wsi.pid, &pid, sizeof(pid));
  memcpy(&wsi.gapp, &app, sizeof(vkcomp));
  memcpy(&wsi.gwc, &wc, sizeof(wclient));

  sa.sa_flags      = SA_SIGINFO;
  sa.sa_sigaction = signal_handler;

  /* ignore whether it works or not */
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
