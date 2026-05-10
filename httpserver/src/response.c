#include "response.h"
#include "mime.h"
#include "server.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

static void send_404(int client_fd) {
  const char *body = "<html><body><h1>404 Not Found</h1></body></html>";
  char header[256];

  snprintf(header, sizeof(header),
           "HTTP/1.1 404 Not Found\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %zu\r\n"
           "Connection: close\r\n"
           "\r\n",
           strlen(body));

  send(client_fd, header, strlen(header), 0);
  send(client_fd, body, strlen(body), 0);
}

static void send_405(int client_fd) {
  const char *body =
      "<html><body><h1>405 Method Not Allowed</h1></body></html>";
  char header[256];

  snprintf(header, sizeof(header),
           "HTTP/1.1 405 Method Not Allowed\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %zu\r\n"
           "Connection: close\r\n"
           "\r\n",
           strlen(body));

  send(client_fd, header, strlen(header), 0);
  send(client_fd, body, strlen(body), 0);
}

void response_send(int client_fd, Request *req) {
  if (req == NULL) {
    send_404(client_fd);
    return;
  }

  if (strcmp(req->method, "GET") != 0) {
    send_405(client_fd);
    return;
  }

  char filepath[512];
  snprintf(filepath, sizeof(filepath), "%s%s", WWW_DIR, req->path);

  // stat - stat gives file information without opening it
  struct stat st;
  if (stat(filepath, &st) == -1) {
    send_404(client_fd);
    return;
  }

  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    send_404(client_fd);
    return;
  }

  const char *mime = mime_get(filepath);

  char header[512];

  snprintf(header, sizeof(header),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: %s\r\n"
           "Content-Length: %lld\r\n"
           "Connection: close\r\n"
           "\r\n",
           mime, (long long)st.st_size);

  send(client_fd, header, strlen(header), 0);

  char buf[BUFFER_SIZE];
  ssize_t bytes_read;

  while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
    send(client_fd, buf, bytes_read, 0);
  }
  close(fd);
}
