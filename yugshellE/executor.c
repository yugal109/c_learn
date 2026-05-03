#include "executor.h"
#include "builtins.h"
#include "command.h"
#include "fcntl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void setup_redirection(Command *cmd) {
  if (cmd->input_file != NULL) {
    int fd = open(cmd->input_file, O_RDONLY);
    if (fd == -1) {
      perror("yugshell: input rediretion");
      exit(1);
    };
    dup2(fd, 0);
    close(fd);
  };

  if (cmd->output_file != NULL) {
    int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
    int fd = open(cmd->output_file, flags, 0644);
    if (fd == -1) {
      perror("yugshell: output redirection");
      exit(1);
    };
    dup2(fd, 1);
    close(fd);
  }
}

int execute(Command *cmd) {
  pid_t pid;
  int status;

  if (cmd->args[0] == NULL) {
    return 1;
  }

  if (is_builtin(cmd->args)) {
    return run_builtin(cmd->args);
  }

  pid = fork();

  if (pid == 0) {
    setup_redirection(cmd);
    if (execvp(cmd->args[0], cmd->args) == -1) {
      perror("yugshell");
      exit(1);
    }
  } else if (pid < 0) {
    perror("yugshell: fork failed");
  } else {
    waitpid(pid, &status, 0);
  }
  return 1;
}

int execute_pipeline(Pipeline *pipeline) {
  int pipefd[2];
  pid_t pid1, pid2;
  int status;

  if (pipe(pipefd) == -1) {
    perror("yugshell: pipe");
    return 1;
  }

  pid1 = fork();

  if (pid1 == 0) {
    dup2(pipefd[1], 1);
    close(pipefd[0]);
    close(pipefd[1]);
    setup_redirection(pipeline->left);
    if (execvp(pipeline->left->args[0], pipeline->left->args) == -1) {
      perror("yugshelll");
      exit(1);
    }
  }

  pid2 = fork();
  if (pid2 == 0) {
    dup2(pipefd[0], 0);
    close(pipefd[1]);
    close(pipefd[0]);
    setup_redirection(pipeline->right);
    if (execvp(pipeline->right->args[0], pipeline->right->args) == -1) {
      perror("yugshell");
      exit(1);
    }
  }

  close(pipefd[0]);
  close(pipefd[1]);

  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);

  return 1;
}
