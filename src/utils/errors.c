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
  vkcomp *app;
  wclient *wc;
};

/* Can do up to 15 processes */
static struct wlu_sig_info wsi[15];
static int current_loc = 0;

static void signal_handler(int sig) {
  int i = current_loc; /* Represents location */

  wlu_log_me(WLU_DANGER, "[x] Process ID: %d | Received signal: %i", wsi[i].pid, sig);
  wlu_log_me(WLU_DANGER, "[x] Caught and freeing memory for");
  wlu_log_me(WLU_DANGER, "[x] vkcomp struct: %p - %p", &wsi[i].app, wsi[i].app);
  wlu_log_me(WLU_DANGER, "[x] wclient struct: %p - %p", &wsi[i].wc, wsi[i].wc);

  if (wsi[i].app)
    wlu_freeup_vk(wsi[i].app);
  if (wsi[i].wc)
    wlu_freeup_wc(wsi[i].wc);

  wlu_log_me(WLU_SUCCESS, "Successfully freed up allocated memory");

  exit(EXIT_FAILURE);
}

int wlu_watch_me(int sig, int num_called, pid_t pid, vkcomp *app, wclient *wc) {
  if (num_called > 14) {
    wlu_log_me(WLU_DANGER, "[x] num_called can not be greater than 15");
    return EXIT_FAILURE;
  }

  int i = num_called; /* Represents location */
  current_loc = num_called;

  memcpy(&wsi[i].pid, &pid, sizeof(pid));
  memcpy(&wsi[i].app, &app, sizeof(vkcomp));
  memcpy(&wsi[i].wc, &wc, sizeof(wclient));

  /* ignore whether it works or not */
  if (signal(sig, signal_handler) == SIG_IGN)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
