#include "acceptclient.h"
#include "handlerequest.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*Accept the incoming client connection*/
int accept_client(int *client_fd) {
  printf("Incoming client connection\n");
  char req_buf[1024];
  int bytes_read = recv(*client_fd, req_buf, sizeof req_buf, 0);

  if (bytes_read == -1) {
    perror("recv()");
    return 1;
  } else if (bytes_read == 0) {
    printf("Client closed connection");
    return 2;
  }

  /*
    example request:
    request: GET / HTTP/1.1
    Host: localhost:1337
    User-Agent: curl/8.4.0
    Accept: * /*
  */
  int i = 0, j = 0;
  char req_copy[1024];
  char *content_type = NULL;
  char *line, *token, *ept, *method, *http_ver;

  strcpy(req_copy, req_buf);

  line = strtok(req_copy, "\r\n");
  while (line != NULL) {
    // first item in an HTTP 1.1 request is the request-line
    // containing the method, endpoint, aynd http version in that order
    if (i == 0) {
      // obtain the endpoint, method, and http ver
      // from the request line
      token = strtok(line, " ");
      while (token != NULL) {
        if (j == 0) {
          method = token;
        } else if (j == 1) {
          ept = token;
        } else {
          http_ver = token;
        }

        token = strtok(NULL, " ");
        j++;
      }
    }

    // extract the content type header
    if (strstr(line, "Content-Type") != NULL) {
      content_type = line;
    }

    line = strtok(NULL, "\r\n");
    i++;
  }
  printf("Req: %s\n", req_buf);
  handle_request(ept, method, http_ver, client_fd);
  printf("After handle_request\n");
  return 0;
}
