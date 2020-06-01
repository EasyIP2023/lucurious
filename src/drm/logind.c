/**
* This file implements logind integration to acquire/release devices
*
* This file is pretty much an exact copy of what's in wlroots logind session backend:
* https://github.com/swaywm/wlroots/blob/master/backend/session/logind.c
* and in Daniel Stone kms-quads:
* https://gitlab.freedesktop.org/daniels/kms-quads/-/blob/master/logind.c
*/

#define LUCUR_DRM_API
#include <lucom.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define DRM_MAJOR 226

static bool find_session_path(dlu_drm_core *core) {
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  bool ret = true;

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", "/org/freedesktop/login1",
      "org.freedesktop.login1.Manager", "GetSession", &error, &msg, "s", core->session.id) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to get session path: %s", error.message);
    ret = false; goto exit_session_path;
  }

  const char *path = NULL;
  if (sd_bus_message_read(msg, "o", &path) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Could not parse session path: %s", error.message);
    ret = false; goto exit_session_path;
  }

  core->session.path = strdup(path);

exit_session_path:
  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);

  return ret;
}

static bool session_activate(dlu_drm_core *core) {
  bool ret = true;
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", core->session.path,
      "org.freedesktop.login1.Session", "Activate", &error, &msg, "") < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to activate session: %s\n", error.message);
    ret = false; goto exit_active;
  }

exit_active:
  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
  return ret;
}

static bool take_control(dlu_drm_core *core) {
  bool ret = true;
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", core->session.path,
      "org.freedesktop.login1.Session", "TakeControl", &error, &msg, "b", false) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to take control of session: %s", error.message);
    ret = false; goto exit_take_control;
  }

exit_take_control:
  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
  return ret;
}

bool dlu_drm_create_session(dlu_drm_core *core) {

  /* If there's a session active for the current process then just use that */
  if (sd_pid_get_session(getpid(), &core->session.id) == 0) goto start_session;

  /**
  * Find any active sessions for the user.
  * Only if the process isn't part of an active session itself
  */
  if (sd_uid_get_display(getuid(), &core->session.id) < 0) {
    dlu_log_me(DLU_DANGER, "[x] sd_uid_get_display: %s", strerror(-errno));
    dlu_log_me(DLU_DANGER, "[x] Couldn't find an active session");
    return false;
  }

  char *type = NULL; /* Check that the available session is a tty */
  if (sd_session_get_type(core->session.id, &type) < 0) {
    dlu_log_me(DLU_DANGER, "[x] sd_session_get_type: %s", strerror(-errno));
    dlu_log_me(DLU_DANGER, "[x] Couldn't get a tty session type for session '%s'", core->session.id);
    return false; 
  }
  
  if (type[0] != 't' || type[1] != 't' || type[2] != 'y') {
    dlu_log_me(DLU_DANGER, "[x] Unfortunately for you, the available session is not a tty :{");
    free(type); return false;
  }

  free(type);

  char *seat = NULL;
  if (sd_session_get_seat(core->session.id, &seat) < 0) {
    dlu_log_me(DLU_DANGER, "[x] sd_session_get_seat: %s", strerror(-errno));
    return false;
  }

  /* check if return seat var is the defualt seat in systemd */
  if (seat[0] == 's' && seat[1] == 'e' && seat[2] == 'a' && seat[3] == 't' && seat[4] == '0') {
    unsigned vtn;
    /* Check if virtual terminal number exists for this session */
    if (sd_session_get_vt(core->session.id, &vtn) < 0) {
      dlu_log_me(DLU_DANGER, "[x] sd_session_get_vt: %s", strerror(-errno));
      dlu_log_me(DLU_DANGER, "[x] Session not running in virtual terminal");
      free(seat); return false;  
    }
  }

  free(seat);

start_session:
  dlu_log_me(DLU_SUCCESS, "In session: %s", core->session.id);

  /* Connect user to system bus */
  if (sd_bus_default_system(&core->session.bus) < 0) {
    dlu_log_me(DLU_DANGER, "[x] sd_bus_default_system: %s", strerror(-errno));
    dlu_log_me(DLU_DANGER, "[x] Failed to open D-Bus connection");
    return false;
  }
  
  /* get session path */
  if (!find_session_path(core)) return false;

  /* Activate the session */
  if (!session_activate(core)) return false;
 
  if (!take_control(core)) return false;

  dlu_log_me(DLU_SUCCESS, "Logind session successfully loaded");

  return true;
}

void release_session_control(dlu_drm_core *core) {
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", core->session.path,
      "org.freedesktop.login1.Session", "ReleaseControl", &error, &msg, "") < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to release control of session: %s", error.message);
  }

  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
}

bool logind_take_device(dlu_drm_core *core, const char *path) {
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  bool ret = true;

  struct stat st;
  if (stat(path, &st) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to stat: '%s'", path);
    return false;
  }

  /* Perform conversion to see if struct stat device ID is 226 */
  if (major(st.st_rdev) == DRM_MAJOR)
    core->session.has_drm = true;

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", core->session.path, "org.freedesktop.login1.Session",
      "TakeDevice", &error, &msg, "uu", major(st.st_rdev), minor(st.st_rdev)) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to take device '%s': %s", path, error.message);
    ret = false; goto exit_logind_take_dev;
  }

  int paused = 0, fd = 0;
  if (sd_bus_message_read(msg, "hb", &fd, &paused) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to parse D-Bus response for '%s': %s", path, strerror(-errno));
    ret = false; goto exit_logind_take_dev;
  }

  // The original fd seems to be closed when the message is freed
  // so we just clone it.
  core->device.kmsfd = fcntl(fd, F_DUPFD_CLOEXEC, 0);
  if (core->device.kmsfd == UINT32_MAX) {
    dlu_log_me(DLU_DANGER, "[x] Failed to clone file descriptor for '%s': %s", path, strerror(errno));
    ret = false; goto exit_logind_take_dev;
  }

exit_logind_take_dev:
  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
  return ret;
}

void logind_release_device(dlu_drm_core *core) {
  sd_bus_message *msg = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;

  struct stat st;
  if (fstat(core->device.kmsfd, &st) < 0) {
    dlu_log_me(DLU_DANGER, "[x] fstat: %s", strerror(errno));
    return;
  }

  if (sd_bus_call_method(core->session.bus, "org.freedesktop.login1", core->session.path, "org.freedesktop.login1.Session",
      "ReleaseDevice", &error, &msg, "uu", major(st.st_rdev), minor(st.st_rdev)) < 0) {
    dlu_log_me(DLU_DANGER, "[x] Failed to release device '%d': %s\n", core->device.kmsfd, error.message);
  }

  sd_bus_error_free(&error);
  sd_bus_message_unref(msg);
  close(core->device.kmsfd);
  core->device.kmsfd = UINT32_MAX;
}