#include "executor.h"
#include "input.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loop(void) {
  char *line;
  char **args;
  int status;
  do {
    printf("yugshell>: ");
    fflush(stdout);

    line = read_line();
    args = parse_line(line);

    status = execute(args);

    free(line);
    free(args);

  } while (status);
}

int main(void) {
  loop();
  printf("exiting yugshell.\n");
  return 0;
}
