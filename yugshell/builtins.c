#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *builtin_names[] = {"cd", "help", "exit"};

int builtin_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "yugshell: cd: missing argument\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("yugshell: cd");
    }
  }
  return 1;
}

int builtin_help(char **args)
{
  // telling compiler that it won't be used
  (void)args;
  printf("built-in commands:\n");
  printf("  cd [dir]   change directory\n");
  printf("  help       show this message\n");
  printf("  exit       exit yugshell\n");
  printf("all other commands are run as external programs.\n");
  return 1;
}

int builtin_exit(char **args)
{
  (void)args;
  return 0;
}

int is_builtin(char **args)
{
  int i;
  int num_builtins = sizeof(builtin_names) / sizeof(builtin_names[0]);

  for (i = 0; i < num_builtins; i++)
  {
    if (strcmp(args[0], builtin_names[i]) == 0)
    {
      return 1;
    }
  }
  return 0;
}

int run_builtin(char **args)
{
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  if (strcmp(args[0], "help") == 0)
    return builtin_help(args);
  if (strcmp(args[0], "exit") == 0)
    return builtin_exit(args);
  return 1;
}
