#include "mime.h"
#include <stdio.h>
#include <string.h>

typedef struct MimeType {
  const char *ext;
  const char *mime;
} MimeType;

static MimeType mime_types[] = {{"html", "text/html"},
                                {"htm", "text/html"},
                                {"css", "text/css"},
                                {"js", "application/javascript"},
                                {"json", "application/json"},
                                {"png", "image/png"},
                                {"jpg", "image/jpeg"},
                                {"jpeg", "image/jpeg"},
                                {"gif", "image/gif"},
                                {"ico", "image/x-icon"},
                                {"txt", "text/plain"},
                                {"pdf", "application/pdf"},
                                {NULL, NULL}};

const char *mime_get(const char *path) {
  const char *dot = strrchr(path, '.');

  if (dot == NULL)
    return "application/octet-stream";
  dot++;

  int i;

  for (i = 0; mime_types[i].ext != NULL; i++) {
    if (strcmp(dot, mime_types[i].ext) == 0)
      return mime_types[i].mime;
  }
  return "application/octet-stream";
}
