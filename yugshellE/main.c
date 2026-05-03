#include "executor.h"
#include "input.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loop(void) {
  char *line;
  Pipeline *pipeline;
  int status;

  do {
    printf("yugshell>: ");
    fflush(stdout);

    line = read_line();

    if (line == NULL || strlen(line) == 0) {
      free(line);
      break;
    }

    pipeline = parse_pipeline(line);

    if (pipeline->has_pipe) {
      status = execute_pipeline(pipeline);
    } else {
      status = execute(pipeline->left);
    }

    free(line);
    free_pipeline(pipeline);

  } while (status);
}

int main(void) {
  loop();
  printf("exiting yugshell.\n");
  return 0;
}
