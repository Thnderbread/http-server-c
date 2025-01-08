#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "createserversocket.h"

/*Attempt to bind to a socket and begin listening on it*/
int create_server_socket(struct addrinfo *hints, int *server_fd, char *PORT, int CONN_BACKLOG)
{
  int status;    // success or failure of getaddrinfo
  int reuse = 1; // ignore EADDRINUSE error

  struct addrinfo *servinfo; // this will point to the linked list of results
  struct addrinfo *curr;     // this will be used to loop through results from getaddrinfo()

  if ((status = getaddrinfo("127.0.0.1", PORT, hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  for (curr = servinfo; curr != NULL; curr = curr->ai_next)
  {
    // try to create a socket with the current family, socktype & protocol
    // if we fail, print the error and move to the next result
    if ((*server_fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
    {
      perror("socket()");
      continue;
    }

    // ignore any EADDRINUSE errors
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse) == -1)
    {
      perror("setsockopt()");
    }

    // try and bind the address to the socket w/ the addr and addr len
    // note - this is traditionally called 'assigning a name to a socket'
    // if we fail, close the potentially opened socket, print the error, & cont.
    if (bind(*server_fd, curr->ai_addr, curr->ai_addrlen) == -1)
    {
      close(*server_fd);
      perror("bind()");
      continue;
    }
    break; // at this point, a successful connection has been established
  }

  if (curr == NULL)
  {
    // we've hit the end of our results with no successful connection
    fprintf(stderr, "couldn't bind to any sockets\n");
    return 2;
  }

  freeaddrinfo(servinfo); // free results

  // attempt to listen
  if (listen(*server_fd, CONN_BACKLOG) == -1)
  {
    perror("listen()");
    close(*server_fd);
    return 1;
  }

  return 0;
}
