#ifndef DEAMON_H
#define DEAMON_H

int deamonize(char *lockfile);
int log_message(char *filename, char *message);
void redirect_stdout(char *filename, int *out, int *save_out);
void reset_stdout(int *out, int *save_out);

#endif
