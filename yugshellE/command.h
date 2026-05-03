#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
  char **args;
  char *input_file;
  char *output_file;
  int append;

} Command;

#endif
