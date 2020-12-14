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

#include <getopt.h>

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

int main(int argc, char **argv) {
  int opt = 0;
  int8_t track = 0;

  while (1) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version",      no_argument,       NULL,  0  },
      {"help",         no_argument,       NULL,  0  },
      {"pgvl",         no_argument,       NULL,  0  },
      {"pie",          no_argument,       NULL,  0  },
      {"pde",          required_argument, NULL,  0  },
      {"display-info", optional_argument, NULL,  0  },
      {0,              0,                 NULL,  0  }
    };

    opt = getopt_long(argc, argv, "vhlid:", long_options, &option_index);
    if (opt == NEG_ONE) { goto exit_loop; }
    track++;

    switch (opt) {
      case 0:
        if (!strcmp(long_options[option_index].name, "version")) { version_num(); goto exit_loop; }
        if (!strcmp(long_options[option_index].name, "help")) { help_message(argv[0]); goto exit_loop; }
        if (!strcmp(long_options[option_index].name, "pgvl")) print_validation_layers();
        if (!strcmp(long_options[option_index].name, "pie")) print_instance_extensions();
        if (!strcmp(long_options[option_index].name, "display-info")) dlu_print_dconf_info(optarg);
        if (!strcmp(long_options[option_index].name, "pde")) {
          if (optarg) {
            print_device_extensions(ret_dtype(optarg));
          } else {
            dlu_print_msg(DLU_DANGER, "[x] usage example: lucur --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU");
            goto exit_loop;
          }
        }
        break;
      case 1: break;
      case 'v': version_num(); goto exit_loop;
      case 'h': help_message(argv[0]); goto exit_loop;
      case 'l': print_validation_layers(); break;
      case 'i': print_instance_extensions(); break;
      case 'd':
        if (optarg) {
          print_device_extensions(ret_dtype(optarg));
        } else {
          dlu_print_msg(DLU_DANGER, "[x] usage example: lucur --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU");
          goto exit_loop;
        }
        break;
      case '?': break;
      default: break;
    }
  }

exit_loop:
  if (opt == NEG_ONE && track == 0) help_message(argv[0]);
  return EXIT_SUCCESS;
}
