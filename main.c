#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define BYTE_WIDTH 16

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main(int argc, char** argv) {
  // check to ensure at least one argument
  if (argc == 1) {
    fprintf(stderr, "Requires at least one file be specified\n");
    return -1;
  }

  for (size_t i = 1; i < argc; ++i) {
    FILE *f = fopen(argv[i], "r");
    if (f == NULL) {
      fprintf(stderr, "Attempting to open file %s returned [%d]\n", argv[i], errno);
      return errno;
    }

    printf("BEGIN \"%s\"\n", argv[i]);
    
    // read bytes
    unsigned char *buffer = calloc(BYTE_WIDTH, 1);

    while (1) {
      size_t items_read = fread((void *)buffer, 1, BYTE_WIDTH, f);
      for (size_t j = 0; j < items_read; ++j) {
        char *color = "";
        char *suffix = "";
        unsigned char byte = buffer[j]; 
        if (byte > 0) {
          color = ANSI_COLOR_RED;
        }
        if (j < items_read - 1) {
          suffix = " ";
        }
        else {
          suffix = "\n";
        }
        printf("%s%02x%s%s", color, byte, ANSI_COLOR_RESET, suffix);
      } 
      if (items_read < BYTE_WIDTH) {
        break;
      }
    }

    free(buffer);
    
    printf("END \"%s\"\n", argv[i]);
  }
}

