#include "sendresponse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define RESPONSE_HEADER_SIZE 1024

int send_response(
  int request_status,
  int *client_fd,
  char *response_header,
  char *response_body,
  char *response_buffer) {
  if (request_status == 0 && strlen(response_body) > 0) {
    // dynamically calculate the Content-Length
    // for writing the response header

    // coalesce the response header and body into the buffer
    snprintf(
      response_buffer,
      sizeof(char) * (strlen(response_body) + strlen(response_header) + 1),
      "%s%s",
      response_header,
      response_body);
  } else {
    // POST, PUT, or DELETE requests, or failed requests only require headers
    printf("Hello im here but shouldn't be\n");
    snprintf(
      response_buffer,
      sizeof(char) * (strlen(response_header)),
      "%s",
      response_header);
  }

  int len = strlen(response_buffer), bytes_sent;
  bytes_sent = send(*client_fd, response_buffer, len * sizeof(char), 0);

  printf("Bytes sent: %d\n", bytes_sent);

  if (bytes_sent == -1) {
    perror("send()");
    return 1;
  }
  return 0;
}
