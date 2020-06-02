#ifndef DLU_DRM_EDID_H
#define DLU_DRM_EDID_H 

/*
 * Parse the very basic information from the EDID block, as described in
 * edid.c. The EDID parser could be fairly trivially extended to pull
 * more information, such as the mode.
 */
struct edid_info {
  char eisa_id[13];
  char monitor_name[13];
  char pnp_id[5];
  char serial_number[13];
};

struct edid_info *edid_parse(const uint8_t *data, size_t length);

#endif