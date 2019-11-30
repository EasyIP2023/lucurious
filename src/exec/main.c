/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Vincent Davis Jr.
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
#include <exec/vkinfo.h>
#include <exec/helpers.h>
#include <getopt.h>

int main(int argc, char **argv) {
  int c = 0;
  int8_t track = 0;

  while (1) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument,       0,  0  },
      {"help",    no_argument,       0,  0  },
      {"pgvl",    no_argument,       0,  0  },
      {"pie",     no_argument,       0,  0  },
      {"pde",     required_argument, 0,  0  },
      {0,         0,                 0,  0  }
    };

    c = getopt_long(argc, argv, "vhlid:d",
        long_options, &option_index);

    if (c == -1) { goto exit_loop; }
    track++;

    switch (c) {
      case 0:
        if (!strcmp(long_options[option_index].name, "version")) { version_num(); goto exit_loop; }
        if (!strcmp(long_options[option_index].name, "help")) { help_message(); goto exit_loop; }
        if (!strcmp(long_options[option_index].name, "pgvl")) print_gvalidation_layers();
        if (!strcmp(long_options[option_index].name, "pie"))  print_instance_extensions();
        if (!strcmp(long_options[option_index].name, "pde")) {
          if (optarg) {
            print_device_extensions(ret_dtype(optarg));
          } else {
            fprintf(stdout, "[x] usage example: lucur --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU\n");
          }
        }
        break;
      case 1: break;
      case 'v': version_num(); break;
      case 'h': help_message(); break;
      case 'l': print_gvalidation_layers(); break;
      case 'i': print_instance_extensions(); break;
      case 'd':
        if (optarg) {
          print_device_extensions(ret_dtype(optarg));
        } else {
          fprintf(stdout, "[x] usage example: lucur --pde VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU\n");
        }
        break;
      case '?': break;
      default: fprintf(stdout, "[x] getopt returned character code 0%o ??\n", c); break;
    }
  }

  if (optind < argc) {
    printf("[x] non-option ARGV-elements: ");
    while (optind < argc)
      printf("[x] %s ", argv[optind++]);
    printf("\n");
    printf("[x] Type lucur --help for help\n");
  }

exit_loop:
  if (c == -1 && track == 0) help_message();
  return EXIT_SUCCESS;
}
