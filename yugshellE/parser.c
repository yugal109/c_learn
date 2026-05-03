#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_COUNT 8

Command *parse_line(char *line) {
  Command *cmd = malloc(sizeof(Command));
  if (cmd == NULL) {
    fprintf(stderr, "yugshell: memeory allocaiton failed\n");
    exit(1);
  }

  cmd->args = malloc(INITIAL_TOKEN_COUNT * sizeof(char *));
  cmd->input_file = NULL;
  cmd->output_file = NULL;
  cmd->append = 0;

  if (cmd->args == NULL) {
    fprintf(stderr, "yugshell: memory aloocation failed\n");
    exit(1);
  }

  int position = 0;
  int bufsize = INITIAL_TOKEN_COUNT;
  char *token = strtok(line, " \t\r\n\a");

  while (token != NULL) {
    if (strcmp(token, ">") == 0) {
      token = strtok(NULL, " \t\r\n\a");
      if (token != NULL) {
        cmd->output_file = token;
      }
      cmd->append = 0;
    } else if (strcmp(token, ">>") == 0) {
      token = strtok(NULL, " \t\r\n\a");
      if (token != NULL) {
        cmd->output_file = token;
      }
      cmd->append = 1;
    } else if (strcmp(token, "<") == 0) {
      token = strtok(NULL, " \t\r\n\a");
      if (token != NULL) {
        cmd->input_file = token;
      }
    } else {
      if (position >= bufsize) {
        bufsize *= 2;
        cmd->args = realloc(cmd->args, bufsize * sizeof(char *));
        if (cmd->args == NULL) {
          fprintf(stderr, "yugshell: memeory allocation failed\n");
          exit(1);
        }
      }
      cmd->args[position] = token;
      position++;
    }
    token = strtok(NULL, " \t\r\n\a");
  }
  cmd->args[position] = NULL;
  return cmd;
}

void free_command(Command *cmd) {
  if (cmd == NULL)
    return;
  free(cmd->args);
  free(cmd);
}

Pipeline *parse_pipeline(char *line) {
  Pipeline *pipeline = malloc(sizeof(Pipeline));
  if (pipeline == NULL) {
    fprintf(stderr, "yugshell: memory aloocation failedl\n");
    exit(1);
  }
  pipeline->has_pipe = 0;
  pipeline->left = NULL;
  pipeline->right = NULL;

  char *pipe_pos = strchr(line, '|');
  if (pipe_pos == NULL) {
    pipeline->left = parse_line(line);
    return pipeline;
  }
  pipeline->has_pipe = 1;
  *pipe_pos = '\0';

  pipeline->left = parse_line(line);
  pipeline->right = parse_line(pipe_pos + 1);

  return pipeline;
}

void free_pipeline(Pipeline *pipeline) {
  if (pipeline == NULL)
    return;
  free_command(pipeline->left);
  free_command(pipeline->right);
  free(pipeline);
}
