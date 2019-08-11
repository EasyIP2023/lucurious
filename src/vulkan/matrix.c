#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <wlu/vlucur/matrix.h>
#include <wlu/utils/log.h>
#include <cglm/mat4.h>

void wlu_set_perspective(
  vkcomp *app,
  float fovy,
  float aspect,
  float nearVal,
  float farVal
) {
  glmc_perspective(glm_rad(fovy), aspect, nearVal, farVal, app->proj);
}

void wlu_set_lookat(vkcomp *app, float dir[3], float eye[3], float up[3]) {
  vec3 loc = { dir[0], dir[1], dir[2] },
       looks = { eye[0], eye[1], eye[2] },
       heads_up = { up[0], up[1], up[2] };

  //glm_vec3_add(loc, looks, center);
  glm_lookat(loc, looks, heads_up, app->view);
  // glm_look(loc, looks, heads_up, app->view);
}

void wlu_set_model_matrix(vkcomp *app, float m) {
  mat4 model = {
    { m, m, m, m },
    { m, m, m, m },
    { m, m, m, m },
    { m, m, m, m },
  };
  memcpy(&app->model, &model, sizeof(mat4));
}

/*
 * A four dimensional vector that is turned into a normalized
 * device coordinates by dividing the whole vector by
 * its last component
 */
void wlu_set_clip_matrix(vkcomp *app, float model[4][4]) {
  mat4 m = {
    { model[0][0], model[0][1], model[0][2], model[0][3] },
    { model[1][0], model[1][1], model[1][2], model[1][3] },
    { model[2][0], model[2][1], model[2][2], model[2][3] },
    { model[3][0], model[3][1], model[3][2], model[3][3] }
  };
  memcpy(&app->clip, &m, sizeof(mat4));
}

void wlu_set_mvp_matrix(vkcomp *app) {
  if (!app->clip) {
    wlu_log_me(WLU_DANGER, "[x] Clip Matrix not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_clip_matrix(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return;
  }

  if (!app->proj) {
    wlu_log_me(WLU_DANGER, "[x] Proection Matrix not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_perspective(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return;
  }

  if (!app->view) {
    wlu_log_me(WLU_DANGER, "[x] View Matrix not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_lookat(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return;
  }

  if (!app->model) {
    wlu_log_me(WLU_DANGER, "[x] Model Matrix not setup");
    wlu_log_me(WLU_DANGER, "[x] Must make a call to wlu_set_model_matrix(3)");
    wlu_log_me(WLU_DANGER, "[x] See man pages for further details");
    return;
  }

  glm_mat4_mulN((mat4 *[]){&app->clip, &app->proj, &app->view, &app->model}, 4, app->mvp);
}

void wlu_print_matrices(vkcomp *app) {
  wlu_log_me(WLU_INFO, "Model Matrix");
  wlu_log_me(WLU_INFO, "Mapping object's local coordinate space into world space");
  glm_mat4_print(app->model, stdout);
  wlu_log_me(WLU_INFO, "View Matrix");
  wlu_log_me(WLU_INFO, "View from world space to camera space");
  glm_mat4_print(app->view, stdout);
  wlu_log_me(WLU_INFO, "Perspective Matrix");
  wlu_log_me(WLU_INFO, "Projection from camera to screen");
  glm_mat4_print(app->proj, stdout);
  wlu_log_me(WLU_INFO, "Clip Matrix");
  glm_mat4_print(app->clip, stdout);
  wlu_log_me(WLU_INFO, "MVP Matrix");
  glm_mat4_print(app->mvp, stdout);
}
