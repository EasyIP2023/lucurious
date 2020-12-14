/* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/input.c */

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

#define LUCUR_DISPLAY_API
#include <lucom.h>

static int open_restricted(const char *path, int UNUSED flags, void *user_data) {
  return logind_take_device((dlu_disp_core *) user_data, path);
}

static void close_restricted(int  fd, void *user_data) {
  logind_release_device(fd, (dlu_disp_core *) user_data);
}

bool dlu_input_create(dlu_disp_core *core) {

  core->input.udev = udev_new();
  if (!core->input.udev) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create a udev context");
    return false;
  }

  core->input.inp = libinput_udev_create_context(&(struct libinput_interface) { .open_restricted = open_restricted, .close_restricted = close_restricted } , core, core->input.udev);
  if (!core->input.inp) {
    dlu_log_me(DLU_DANGER, "[x] Failed to create libinput context");
    return false;
  }

  if (libinput_udev_assign_seat(core->input.inp, "seat0") == NEG_ONE) {
    dlu_log_me(DLU_DANGER, "[x] Failed to assign seat0 to libinput context");
    return false;
  }

  return true;
}

bool dlu_input_retrieve(dlu_disp_core *core, uint32_t *key_code) {
  enum libinput_event_type type;
  struct libinput_event *event = NULL;

  /* Read events from libinput FDs and processes them */
  if (libinput_dispatch(core->input.inp)) {
    dlu_log_me(DLU_DANGER, "[x] libinput_dispatch: %s", strerror(errno));
    return false;
  }
 
  event = libinput_get_event(core->input.inp); 
  if (!event) goto end_func;

  type = libinput_event_get_type(event);
  switch (type) {
    case LIBINPUT_EVENT_KEYBOARD_KEY:
      {
        struct libinput_event_keyboard *key_event = libinput_event_get_keyboard_event(event);
        *key_code = libinput_event_keyboard_get_key(key_event);
      }
      break;
    default: break;
  }
     
  libinput_event_destroy(event);

end_func:
  return true;
}

int dlu_input_retrieve_fd(dlu_disp_core *core) { return libinput_get_fd(core->input.inp); }
