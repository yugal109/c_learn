#include "executor.h"
#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int execute(char **args)
{
  pid_t pid;
  int status;

  if (args[0] == NULL)
  {
    return 1;
  }

  if (is_builtin(args))
  {
    return run_builtin(args);
  }

  pid = fork();

  if (pid == 0)
  {
    if (execvp(args[0], args) == -1)
    {
      perror("yugshell");
      exit(1);
    }
  }
  else if (pid < 0)
  {
    perror("yugshell: fork failed");
  }
  else
  {
    waitpid(pid, &status, 0);
  }
  return 1;
}
