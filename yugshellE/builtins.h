#ifndef BUILTINS_H
#define BUILTINS_H

int builtin_cd(char **args);
int builtin_help(char **args);
int builtin_exit(char **args);
int builtin_history(char **args);

int is_builtin(char **args);
int run_builtin(char **args);

#endif
