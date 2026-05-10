#include "server.h"
#include "request.h"
#include "response.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int create_server_socket(void) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    exit(1);
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(server_fd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  return server_fd;
}

void server_start(void) {
  int server_fd = create_server_socket();
  printf("yugserver listening on http://localhost:%d\n", PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd == -1) {
      perror("accept");
      continue;
    }

    printf("connection from %s\n", inet_ntoa(client_addr.sin_addr));

    char buf[BUFFER_SIZE];
    memset(buf, 0, sizeof(buf));

    ssize_t bytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (bytes > 0) {
      printf("request:\n%s\n", buf);

      Request req;
      if (request_parse(buf, &req) == 0) {
        response_send(client_fd, &req);
      } else {
        response_send(client_fd, NULL);
      }
    }
    close(client_fd);
  }
}
