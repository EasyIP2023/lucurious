#include <lucom.h>
#include <wlu/vlucur/vkall.h>
#include <exec/vkinfo.h>
#include <exec/helpers.h>
#include <getopt.h>

int main(int argc, char **argv) {
  int c;

  while (1) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument,       0,  0  },
      {"help",  no_argument,         0,  0  },
      {"pgvl",  no_argument,         0,  0  },
      {"pie",   no_argument,         0,  0  },
      {"pde",   required_argument,   0,  0  },
      {0,         0,                 0,  0  }
    };

    c = getopt_long(argc, argv, "vhlid:d",
        long_options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        if (!strcmp(long_options[option_index].name, "version")) version_num();
        if (!strcmp(long_options[option_index].name, "help")) help_message();
        if (!strcmp(long_options[option_index].name, "pgvl")) print_gvalidation_layers();
        if (!strcmp(long_options[option_index].name, "pie")) print_instance_extensions();
        if (!strcmp(long_options[option_index].name, "pde")) {
          if (optarg) {
            char **tokens = str_split(optarg, ',');
            if (tokens) {
              print_device_extensions(ret_qfambit(*tokens), ret_dtype(*(tokens + 1)));
              free(*tokens); free(*(tokens + 1));
              free(tokens);
              tokens = NULL;
            } else {
              fprintf(stdout, "[x] usage example: lucur --pde VK_QUEUE_GRAPHICS_BIT,1\n");
              fprintf(stdout, "[x] must add ',' to delimit the string\n");
            }
          }
        }
        break;
      case 1:
        break;
      case 'v':
        version_num();
        break;
      case 'h':
        help_message();
        break;
      case 'l':
        print_gvalidation_layers();
        break;
      case 'i':
        print_instance_extensions();
        break;
      case 'd':
        if (optarg) {
          char **tokens = str_split(optarg, ',');
          if (tokens) {
            print_device_extensions(ret_qfambit(*tokens), ret_dtype(*(tokens + 1)));
            free(*tokens); free(*(tokens + 1));
            free(tokens);
            tokens = NULL;
          } else {
            fprintf(stdout, "[x] usage example: lucur --pde VK_QUEUE_GRAPHICS_BIT,1\n");
            fprintf(stdout, "[x] must add ',' to delimit the string\n");
          }
        }
        break;
      case '?':
        break;
      default:
        printf("[x] getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) {
    printf("[x] non-option ARGV-elements: ");
    while (optind < argc)
      printf("[x] %s ", argv[optind++]);
    printf("\n");
    printf("[x] Type lucur --help for help\n");
  }

  return EXIT_SUCCESS;
}
