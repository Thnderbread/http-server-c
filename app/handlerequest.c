#include "handlerequest.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "handleget.h"
#include "sendresponse.h"
#include "utils.h"

int handle_request(char *ept, char *method, char *http_ver, int *client_fd) {
  int request_status;
  int http_status_code;
  int response_body_size = 4096;
  int response_header_size = 1024;

  char content_type[100];
  char response_body[response_body_size];
  char response_header[response_header_size];
  char response_buffer[response_header_size + response_body_size];

  if (strcmp(http_ver, "HTTP/1.1") != 0) {
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 505 Version Not Supported\r\n\r\n");
  }

  // ! Figure out forking

  if (strcmp(method, "GET") == 0) {
    request_status = handle_get(
      ept, content_type, response_body, &http_status_code, response_body_size);
  } else if (strcmp(method, "POST") == 0) {
    /* POST handler function */
    printf("POST Handler triggered\n");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 501 Not Implemented\r\n\r\n");
  } else if (strcmp(method, "PUT") == 0) {
    /* PUT handler function */
    printf("PUT Handler triggered\n");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 501 Not Implemented\r\n\r\n");

  } else if (strcmp(method, "DELETE") == 0) {
    /* DELETE handler function */
    printf("DELETE Handler triggered\n");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 501 Not Implemented\r\n\r\n");

  } else if (strcmp(method, "OPTIONS") == 0) {
    printf("OPTIONS Handler triggered\n");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 200 OK\rnAllow: OPTIONS, GET, PUT, POST, DELETE\r\n\r\n");
  } else {
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
  }

  long int content_length = sizeof(char) * strlen(response_body);
  write_response_header(
    http_status_code, content_length, response_header, content_type);
  int response_status = send_response(
    request_status, client_fd, response_header, response_body, response_buffer);

  if (response_status != 0) {
    close(*client_fd);
    return 1;
  }

  close(*client_fd);
  printf("Closed client fd\n");
  return 0;
}
