#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define BYTE_WIDTH 0x1000

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct Printer {
  int width;
  bool colored_output;
  bool display_text;
} Printer;

typedef struct Args {
  char **files;
  size_t num_files;

  int width;
  bool colored_output;
  bool display_text;
} Args;

Args parse_args(int argc, char **argv) {
  Args ret_args = {
    .files = calloc(argc, sizeof(char *)),
    .num_files = 0,
    .width = 16,
    .colored_output = false,
    .display_text = false
  };

  for (size_t i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "--color") == 0) {
      ret_args.colored_output = true;
    } else if (strcmp(argv[i], "--text") == 0) {
      ret_args.display_text = true;
    } else if (strcmp(argv[i], "--width") == 0) {
      ++i;
      if (i >= argc) {
        fprintf(stderr, "--width must be specified\n");
        exit(1);
      }
      int width = atoi(argv[i]);
      if (width <= 0) {
        fprintf(stderr, "--width must be an integer > 0\n");
        exit(1);
      }
      ret_args.width = atoi(argv[i]);
    } else {
      ret_args.files[ret_args.num_files++] = argv[i];
    }
  }

  return ret_args;
}

int main(int argc, char **argv) {
  Args args = parse_args(argc, argv);

  // check to ensure at least one argument
  if (argc == 1) {
    fprintf(stderr, "Requires at least one file be specified\n");
    return -1;
  }

  for (size_t i = 1; i < args.num_files; ++i) {
    FILE *f = fopen(args.files[i], "r");
    if (f == NULL) {
      fprintf(stderr, "Attempting to open file %s returned [%d]\n", args.files[i], errno);
      return errno;
    }

    printf("BEGIN \"%s\"\n", args.files[i]);
    
    // read bytes
    unsigned char *buffer = calloc(BYTE_WIDTH, 1);
    char *characters = calloc(args.width + 1, 1);

    while (1) {
      size_t items_read = fread((void *)buffer, 1, BYTE_WIDTH, f);
      printf("%07x\t", 0);
      for (size_t j = 0; j < items_read; ++j) {
        char *color = "";
        unsigned char byte = buffer[j]; 
        if (byte >= 0x20 && byte < 0x7E) {
          characters[j % args.width] = (char) byte;
        }
        else {
          characters[j % args.width] = '.';
        }
        if (byte > 0 && args.colored_output) {
          color = ANSI_COLOR_RED;
        }
        if ((j + 1) % args.width == 0 || (j + 1 == items_read && items_read < BYTE_WIDTH)) {
          if (args.display_text) {
            printf("%s%02x%s|%s\n%07x\t", color, byte, ANSI_COLOR_RESET, characters, j + 1); 
            memset(characters, 0, args.width + 1);
          }
          else {
            printf("%s%02x%s\n", color, byte, ANSI_COLOR_RESET);
          }
        }
        else {
          printf("%s%02x%s ", color, byte, ANSI_COLOR_RESET);
        }
      } 
      if (items_read < BYTE_WIDTH) {
        break;
      }
    }

    free(buffer);
    free(characters);
    
    printf("END \"%s\"\n", argv[i]);
  }
}

