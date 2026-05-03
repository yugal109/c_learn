#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_COUNT 8

char **parse_line(char *line) {
  int bufsize = INITIAL_TOKEN_COUNT;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (tokens == NULL) {
    fprintf(stderr, "yugshell: memory allocation filed\n");
    exit(1);
  }

  token = strtok(line, " \t\r\n\a");

  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize *= 2;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (tokens == NULL) {
        fprintf(stderr, "yugshell: memory allocation failed\n");
        exit(1);
      }
    }
    token = strtok(NULL, " \t\r\n\a");
  }

  tokens[position] = NULL;
  return tokens;
}
