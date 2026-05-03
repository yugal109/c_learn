#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "command.h"
#include "parser.h"

int execute(Command *cmd);
int execute_pipeline(Pipeline *pipeline);

#endif
