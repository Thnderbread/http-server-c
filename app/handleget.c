#include "handleget.h"
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const char kPathSeparator =
#ifdef _WIN32
  '\\';
#else
  '/';
#endif

/*Handle the http request
return 0 if success
return 1 if some error happened, 500 class http error
return 2 if something like 404 is being returned
*/
int handle_get(
  char *ept,
  char *response_body,
  char *response_header,
  int response_body_size,
  int response_header_size) {

  char *ept_dupe = strdup(ept); // changing to index.html if ept is '/'
  // handle page requests
  printf("GET Handler triggered, requested endpoint: %s\n", ept_dupe);
  // handling JSON api epts
  if (strstr(ept_dupe, "/api/")) {
    // if we handle json
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 501 NOT IMPLEMENTED\r\nConnection: close\r\n\r\n");
    return 1;
  }

  // handle static file requests
  char cwd[PATH_MAX]; // get cwd for dist directory
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd()");
    snprintf(
      response_body,
      response_body_size,
      "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nConnection: close\r\n\r\n");
    return 1;
  }

  // request index page if requested path is '/'
  ept_dupe = strcmp(ept_dupe, "/") == 0 ? "/index.html" : ept_dupe;

  // construct path to dist dir
  char *dist_dirname = "dist";
  char dist_dir[PATH_MAX + 1];
  snprintf(
    dist_dir,
    PATH_MAX + 1,
    "%s%c%s%s",
    cwd,
    kPathSeparator,
    dist_dirname,
    ept_dupe);

  // look for the file to serve
  char resolved_path[PATH_MAX];
  if (realpath(dist_dir, resolved_path) == NULL) {
    perror("realpath()");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 404 NOT FOUND\r\nConnection: close\r\n\r\n");
    return 2;
  }

  FILE *f;
  f = fopen(resolved_path, "r");

  if (f == NULL) {
    perror("fopen()");
    snprintf(
      response_header,
      response_header_size,
      "HTTP/1.1 404 NOT FOUND\r\nConnection: close\r\n\r\n");
    return 2;
  }

  struct stat stats;
  fstat(fileno(f), &stats);

  size_t bytesread;
  do {
    bytesread =
      fread(response_body, sizeof(char), stats.st_size / sizeof(char), f);
  } while (bytesread > 0);

  fclose(f);
  return 0;
}
