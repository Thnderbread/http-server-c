#ifndef CREATESERVERSOCKET_H_
#define CREATESERVERSOCKET_H_

/*Attempt to bind to a socket and begin listening on it*/
int create_server_socket(struct addrinfo *hints, int *server_fd, char *PORT, int CONN_BACKLOG);

#endif
