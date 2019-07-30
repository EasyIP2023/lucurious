#ifndef WLU_FILE_H
#define WLU_FILE_H

typedef struct wlu_file_info {
  char *bytes;
  long byte_size;
} wlu_file_info;

wlu_file_info wlu_read_file(const char *filename);

#endif
