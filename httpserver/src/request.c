#include "request.h"
#include <stdio.h>
#include <string.h>

int request_parse(const char *raw, Request *req) {

  if (raw == NULL || req == NULL)
    return -1;

  // HTTP request first line looks like:
  // GET /index.html HTTP/1.1\r\n
  if (sscanf(raw, "%15s %255s %15s", req->method, req->path, req->version) != 3)
    return -1;

  // only support get for now
  if (strcmp(req->method, "GET") != 0)
    return -1;

  // if the path is just / serve index.html
  if (strcmp(req->path, "/") == 0)
    snprintf(req->path, sizeof(req->path), "%s", "/index.html");

  return 0;
}
