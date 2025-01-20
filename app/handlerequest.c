#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "handleget.h"
#include "handlerequest.h"

int handle_request(char *ept, char *method, char *http_ver, int *client_fd)
{
  // send response
  char *response;
  if (strcmp(http_ver, "HTTP/1.1") != 0)
  {
    response = "HTTP/1.1 505 Version Not Supported\r\n\r\n";
  }

  if (strcmp(method, "GET") == 0)
  {
    /* GET handler function */
    int response_header_size = 100;
    int response_buffer_size = 4096;

    char response_header[response_header_size];
    char *response_buffer = malloc(response_buffer_size);

    int res = handle_get(ept, response_buffer);
    char super_response[response_header_size + response_buffer_size];

    // if handle_get succeeded, need to write the header & body separately
    if (res == 0)
    {
      // using snprintf to dynamically calculate the Content-Length
      // for writing the response header
      snprintf(response_header, response_header_size, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", sizeof(char) * strlen(response_buffer));

      // using snprintf to coalesce the response header and body
      snprintf(super_response, sizeof(char) * (strlen(response_buffer) + strlen(response_header)), "%s%s", response_header, response_buffer);
    }
    else
    {
      // directly send the response if shit failed type shit
      snprintf(super_response, sizeof(response_buffer), "%s", response_buffer);
    }

    int len = strlen(super_response);
    printf("len: %d\n", len);
    free(response_buffer);

    int bytes_sent;
    printf("arg: %ld", len * sizeof(char) + 1);

    bytes_sent = send(*client_fd, super_response, len * sizeof(char) + 1, 0);

    if (bytes_sent == -1)
    {
      perror("send()");
      return 1;
    }

    close(*client_fd);
    return 0;
  }
  else if (strcmp(method, "POST") == 0)
  {
    /* POST handler function */
    printf("POST Handler triggered\n");
    response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
  }
  else if (strcmp(method, "PUT") == 0)
  {
    /* PUT handler function */
    printf("PUT Handler triggered\n");
    response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
  }
  else if (strcmp(method, "DELETE") == 0)
  {
    /* DELETE handler function */
    printf("DELETE Handler triggered\n");
    response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
  }
  else
  {
    response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
  }

  int len, bytes_sent;
  printf("here\n");
  bytes_sent = send(*client_fd, response, len, 0);

  if (bytes_sent == -1)
  {
    perror("send()");
    return 1;
  }

  close(*client_fd);
  return 0;
}
