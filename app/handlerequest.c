#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
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
    printf("GET Handler triggered\n");
    response = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/plain\r\n\r\n";
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
  len = strlen(response);
  bytes_sent = send(*client_fd, response, len, 0);

  if (bytes_sent == -1)
  {
    perror("send()");
    return 1;
  }

  close(*client_fd);
  return 0;
}
