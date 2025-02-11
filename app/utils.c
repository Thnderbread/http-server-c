#include "utils.h"
#include <stdio.h>

// ! Finish writing this i don't think i finished the 200 path for write_header
// ! Also didn't write the body fn
// ! Test header before body
// ! Also looks really ugly try to fix it

#define RESPONSE_BODY_SIZE 4096
#define RESPONSE_HEADER_SIZE 1024

// ! This fn doesn't need to return anything, a response will be sent no matter
// what happens here ? Nested local fn for snprintf shortcut?
int write_response_header(
  int status_code,
  long int content_length,
  char *response_header,
  char *content_type) {
  const char *status_code_200 = "HTTP/1.1 200 OK";
  const char *status_code_201 = "HTTP/1.1 201 Created";
  const char *status_code_400 = "HTTP/1.1 400 Bad Request";
  const char *status_code_404 = "HTTP/1.1 404 Not Found";
  const char *status_code_405 = "HTTP/1.1 405 Method Not Allowed";
  const char *status_code_500 = "HTTP/1.1 500 Internal Server Error";
  const char *status_code_501 = "HTTP/1.1 501 Not Implemented";
  const char *status_code_505 = "HTTP/1.1 505 Version Not Supported";

  const char *connection_close = "Connection: close";

  if (response_header == NULL) {
    fprintf(stderr, "write_response_header(): response_header is NULL\n");
    snprintf(
      response_header,
      RESPONSE_HEADER_SIZE,
      "%s\r\n%s\r\n\r\n",
      status_code_500,
      connection_close);
    return 1;
  }

  switch (status_code) {
    case 200:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\nContent-Length: %ld\r\n%s\r\n%s\r\n\r\n",
        status_code_200,
        content_length,
        content_type,
        connection_close);
      return 0;
    case 201:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_201,
        connection_close);
      return 0;
    case 400:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_400,
        connection_close);
      return 0;
    case 404:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_404,
        connection_close);
      return 0;
    case 405:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_405,
        connection_close);
      return 0;
    case 500:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_500,
        connection_close);
      return 0;
    case 501:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_501,
        connection_close);
      return 0;
    case 505:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_505,
        connection_close);
      return 0;
    default:
      snprintf(
        response_header,
        RESPONSE_HEADER_SIZE,
        "%s\r\n%s\r\n\r\n",
        status_code_500,
        connection_close);
      return 1;
  }
}

// int write_response_body(char *response_body, char *content_type) {
//   if (response_body == NULL) {
//     return 1;
//   }

//   if (content_type == "application/json") {
//     /* code */
//   }
// }
