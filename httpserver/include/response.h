#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"

void response_send(int client_fd, Request *req);

#endif
