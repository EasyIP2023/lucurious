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

#define LUCUR_VKCOMP_API
#define LUCUR_DISPLAY_API // for definition of dlu_print_dconf_info
#include <lucom.h>

/* In helpers.c */
VkQueueFlagBits ret_qfambit(char *str);
VkPhysicalDeviceType ret_dtype(char *str);
void help_message(const char *file);
void version_num();

/* In vk_info.c */
void print_validation_layers();
void print_instance_extensions();
void print_device_extensions(VkPhysicalDeviceType dt);

int main(int argc, char *argv[]) {
  char *arg = NULL, *val = NULL;

  while ((arg = *(++argv))) {
    if (!strcmp(arg, "version")) { version_num(); break; }
    if (!strcmp(arg, "help")) { help_message(argv[0]); break; }
    if (!strcmp(arg, "pgvl")) { print_validation_layers(); break; }
    if (!strcmp(arg, "pie")) { print_instance_extensions(); break; }
    if (!strcmp(arg, "display-info")) { dlu_print_dconf_info((val = *(++argv))); break; }
    if (!strcmp(arg, "pde")) {
      if ((val = *(++argv))) {
        print_device_extensions(ret_dtype(val));
        break;
      } else {
        dlu_print_msg(DLU_DANGER, "[x] usage example: %s --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU", argv[0]);
        break;
      }
    }
    
    if (!strcmp(arg, "-v")) { version_num(); break; }
    if (!strcmp(arg, "-h")) { help_message(argv[0]); break; }
    if (!strcmp(arg, "-l")) { print_validation_layers(); break; }
    if (!strcmp(arg, "-i")) { print_instance_extensions(); break; }
    if (!strcmp(arg, "-d")) {
      if ((val = *(++argv))) {
        print_device_extensions(ret_dtype(val));
        break;
      } else {
        dlu_print_msg(DLU_DANGER, "[x] usage example: %s -d VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU", argv[0]);
        break;
      }
    }
  }

  return EXIT_SUCCESS;
}
