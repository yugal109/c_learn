#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
  char method[16];
  char path[256];
  char version[16];
} Request;

int request_parse(const char *raw, Request *req);

#endif
