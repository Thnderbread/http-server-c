#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include "acceptclient.h"
#include "createserversocket.h"

#define PORT "1337"
#define CONN_BACKLOG 5

int main() {
  // Disable output buffering
  // Apparently this doesn't quite work
  // because the terminal needs to be
  // configured as well - google termios i think?
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // configuring some information about the server socket
  struct sockaddr_in server_addr;

  int server_fd;
  struct addrinfo hints; // LINK - app\server.c:90

  memset(&hints, 0, sizeof hints); // ensure hints is empty
  hints.ai_flags = AI_NUMERICHOST; // LINK - app\server.c:73
  hints.ai_family = AF_UNSPEC; // tells getaddrinfo to look for ipv4 & 6 addrs
  hints.ai_socktype = SOCK_STREAM; // or SOCK_DGRAM for UDP
  hints.ai_protocol = 0; // automatically choose protocol type (wut dat mean),
                         // guides don't even have this set explicitly so

  // TODO - Haven't tested failure paths here
  int res;
  if (
    (res = create_server_socket(&hints, &server_fd, PORT, CONN_BACKLOG)) >= 1) {
    fprintf(stderr, "couldn't create server socket\n");
    return 1;
  } else {
    printf("Successfully bound to socket, Server's up and running for now\n");
  }

  int client_fd;
  struct sockaddr client_addr; // LINK - app\server.c:109
  socklen_t client_addr_len;

  client_addr_len = sizeof client_addr;
  client_fd = accept(server_fd, &client_addr, &client_addr_len);

  if (client_fd == -1) {
    perror("couldn't accept() new connection");
  } else {
    accept_client(&client_fd);
    printf("Here before death\n");
  }

  close(server_fd);
  return 0;
}

/*
        using AI_PASSIVE means we don't have to hardcode the address during
   bind() call. Since i'm hardcoding localhost, i'm using numeric host, but
   ordinarily a server would want to use AI_PASSIVE
*/

/*
* an addrinfo struct is passed to getaddrinfo
* to help it fill out its results
* (a linked list of addrinfo structs). In Beej's words,
* it is used to "prep the socket address structures for subsequent use."
struct addrinfo {
        int ai_flags;		 // AI_PASSIVE, AI_CANONNAME, ...
        int ai_family;	 // AF_xxx
        int ai_socktype; // SOCK_xxx
        int ai_protocol; // 0 (auto) or IPPROTO_TCP, IPPROTO_UDP

        socklen_t ai_addrlen;			// length of ai_addr
        char *ai_canonname;				// canonical name for
nodename struct sockaddr *ai_addr; // binary address struct addrinfo *ai_next;
// next structure in linked list
};
*/

/*
Holds information for many different types of sockets.
! In this specific case, client_addr would normally have been
! declared as an IPv4 (sockaddr_in) or IPv6 (sockaddr_in6) struct,
! but since I'm not doing anything with IP version specific data I've
! left it as a generic sockaddr.
? Also note, sockaddr differs from sockaddr_storage in that the former
? is for generic, basic storage when specific values are not necessary.
? It's typically just passed around by pointer to other fns like bind(),
? accept(), or connect(), while sockaddr_storage is big enough to
? accommodate different address types. You'd use this when you *do* need
? to access specific values of an ip address, and you aren't sure what
? family of ip address you'll get. Safer when working with fns like
? accept() or getpeername(), which can return an IPv4 or IPv6 address.
struct sockaddr {
                unsigned short    sa_family;    // address family, AF_xxx
(AF_INET | AF_INET6) for IPv4 & IPv6 in this context char sa_data[14];  // 14
bytes of protocol address. Contains destination address & port number for the
socket. Not large enough to accommodate a larger address like IPv6.
}; */

// ! Valgrind! :
/* valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
 * --verbose --log-file=valgrind-out.txt ./a.out */

// ! gcc pre-valgrind! (I forget what the -ggdb3 does.) :
// gcc -ggdb3 app/*.c -o a.out

// ! Address sanitizer! (fuck is that!):
// g++ -fsanitize=address -g3 -std=c17 irbis.cc (input files) -o (out file)
