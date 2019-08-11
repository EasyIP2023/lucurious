#ifndef MATRIX_H
#define MATRIX_H

void wlu_set_perspective(
  vkcomp *app,
  float fovy,
  float aspect,
  float nearVal,
  float farVal
);

void wlu_set_lookat(vkcomp *app, float dir[3], float eye[3], float up[3]);

void wlu_set_model_matrix(vkcomp *app, float m);

void wlu_set_clip_matrix(vkcomp *app, float model[4][4]);

void wlu_set_mvp_matrix(vkcomp *app);

void wlu_print_matrices(vkcomp *app);

#endif
