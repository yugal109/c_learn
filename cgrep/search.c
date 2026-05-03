#include "search.h"
#include <stdio.h>
#include <string.h>

void search_in_file(const char *filename, const char *pattern, int show_lines) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "cgrep: %s: No such file or directory\n", filename);
    return;
  }

  char line[1024];
  int line_number = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;
    if (strstr(line, pattern) != NULL) {
      if (show_lines) {
        printf("%d: %s", line_number, line);
      } else {
        printf("%s", line);
      }
    }
  }
  fclose(file);
}
