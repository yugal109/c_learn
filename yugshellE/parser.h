#ifndef PARSER_H
#define PARSER_H

#include "command.h"

typedef struct {
  Command *left;
  Command *right;
  int has_pipe;
} Pipeline;

Command *parse_line(char *line);
Pipeline *parse_pipeline(char *line);

void free_command(Command *cmd);
void free_pipeline(Pipeline *pipeline);

#endif
