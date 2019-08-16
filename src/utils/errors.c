/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucurious Labs
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

  wlu_log_me(WLU_DANGER, "");
  wlu_log_me(WLU_DANGER, "[x] Process ID: %d | Received signal: %i", wsi[i].pid, sig);
  wlu_log_me(WLU_DANGER, "[x] Caught and freeing memory for");
  wlu_log_me(WLU_DANGER, "[x] vkcomp struct: %p - %p", &wsi[i].app, wsi[i].app);
  wlu_log_me(WLU_DANGER, "[x] wclient struct: %p - %p", &wsi[i].wc, wsi[i].wc);

  if (wsi[i].app)
    wlu_freeup_vk(wsi[i].app);
  if (wsi[i].wc)
    wlu_freeup_wc(wsi[i].wc);

  wlu_log_me(WLU_SUCCESS, "Successfully freed up most allocated memory :)");

  exit(EXIT_FAILURE);
}

int wlu_watch_me(int sig, int num_called, pid_t pid, void *app, void *wc) {
  if (num_called > 14) {
    wlu_log_me(WLU_DANGER, "[x] num_called can not be greater than 15");
    return EXIT_FAILURE;
  }

  int i = num_called; /* Represents location */
  current_loc = num_called;

  wsi[i].pid = pid;
  wsi[i].app = (vkcomp *) app;
  wsi[i].wc  = (wclient *) wc;

  /* ignore whether it works or not */
  if (signal(sig, signal_handler) == SIG_IGN)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
