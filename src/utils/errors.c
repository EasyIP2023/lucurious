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
#include <wlu/vlucur/gp.h>
#include <wlu/wclient/client.h>
#include <wlu/shader/shade.h>
#include <wlu/utils/log.h>

#include <wlu/utils/errors.h>

#include <signal.h>

/*
 * global struct used to copy data over
 * and pass it into the signal handler
 * I'm assuming that the user called the signal
 * handler each time a new process is created
 */

static struct wlu_sig_info {
  pid_t pid;

  uint32_t app_pos;
  vkcomp **app;

  uint32_t wc_pos;
  wclient **wc;

  uint32_t shader_mod_pos;
  struct app_shader {
    vkcomp *app;
    VkShaderModule *shader_mod;
  } *apsh;
} wsi;

static void signal_handler(int sig) {

  wlu_log_me(WLU_DANGER, "");
  wlu_log_me(WLU_DANGER, "[x] Process ID: %d | Received signal: %i", wsi.pid, sig);
  wlu_log_me(WLU_DANGER, "[x] Caught and freeing memory for");

  for (uint32_t i = 0; i < wsi.shader_mod_pos; i++) {
    if (wsi.apsh && wsi.apsh[i].shader_mod) {
      wlu_log_me(WLU_DANGER, "[x] shader module: %p", wsi.apsh[i].shader_mod);
      wlu_freeup_shader(wsi.apsh[i].app, wsi.apsh[i].shader_mod);
    }
  }

  if (wsi.apsh) {
    free(wsi.apsh);
    wsi.apsh = NULL;
  }

  for (uint32_t i = 0; i < wsi.app_pos; i++) {
    if (wsi.app && wsi.app[i]) {
      wlu_log_me(WLU_DANGER, "[x] vkcomp struct: %p - %p", &wsi.app[i], wsi.app[i]);
      wlu_freeup_vk(wsi.app[i]);
    }
  }

  if (wsi.app) {
    free(wsi.app);
    wsi.app = NULL;
  }

  for (uint32_t i = 0; i < wsi.wc_pos; i++) {
    if (wsi.wc && wsi.wc[i]) {
      wlu_log_me(WLU_DANGER, "[x] wclient struct: %p - %p", &wsi.wc[i], wsi.wc[i]);
      wlu_freeup_wc(wsi.wc[i]);
    }
  }

  if (wsi.wc) {
    free(wsi.wc);
    wsi.wc = NULL;
  }

  wlu_log_me(WLU_SUCCESS, "Successfully freed up most allocated memory :)");

  exit(EXIT_FAILURE);
}

int wlu_watch_me(int sig, pid_t pid) {
  wsi.pid = pid;

  /* ignore whether it works or not */
  if (signal(sig, signal_handler) == SIG_IGN)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

/* Leave this the way it is future me... */
void wlu_add_watchme_info(
  uint32_t app_pos,
  vkcomp *app,
  uint32_t wc_pos,
  wclient *wc,
  uint32_t shader_mod_pos,
  VkShaderModule *shader_mod
) {

  if (app) {
    wsi.app_pos = app_pos;
    wsi.app = realloc(wsi.app, wsi.app_pos * sizeof(vkcomp));
    wsi.app[wsi.app_pos-1] = app;
  }

  if (wc) {
    wsi.wc_pos = wc_pos;
    wsi.wc = realloc(wsi.wc, wsi.wc_pos * sizeof(wclient));
    wsi.wc[wsi.wc_pos-1] = wc;
  }

  if (shader_mod) {
    wsi.shader_mod_pos = shader_mod_pos;
    wsi.apsh = realloc(wsi.apsh, wsi.shader_mod_pos * sizeof(struct app_shader));
    wsi.apsh[wsi.shader_mod_pos-1].app = app;
    wsi.apsh[wsi.shader_mod_pos-1].shader_mod = shader_mod;
  }
}
