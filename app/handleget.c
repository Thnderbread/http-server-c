#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "handleget.h"

char *GET_ROUTES[] = {"/", "/index.html", "users", "foo"};
const char kPathSeparator =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

// Should I respond here or offload that to another function?
// I think I'll offload it
int handle_get(char *ept, char *response_buffer)
{
  // may need to re-assign this to index.html if it's just '/'
  char *ept_dupe = strdup(ept);
  // handle page requests
  printf("GET Handler triggered, requested endpoint: %s\n", ept_dupe);
  if (strcmp(ept_dupe, GET_ROUTES[0]) == 0 || strcmp(ept_dupe, GET_ROUTES[1]) == 0)
  {
    struct stat stats;

    // get cwd
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
      perror("getcwd()");
      sprintf(response_buffer, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nConnection: close\r\n\r\n");
      return 1;
    }

    // request index page if requested path is '/'
    if (strcmp(ept_dupe, "/") == 0)
    {
      ept_dupe = "/index.html";
    }

    // construct path to dist dir
    char *dist_dirname = "dist";
    char dist_dir[PATH_MAX + 1];
    snprintf(dist_dir, PATH_MAX + 1, "%s%c%s%s", cwd, kPathSeparator, dist_dirname, ept_dupe);

    // look for the file to serve
    char resolved_path[PATH_MAX];
    if (realpath(dist_dir, resolved_path) == NULL)
    {
      printf("constructed path: %s\n", dist_dir);
      perror("realpath()");
      sprintf(response_buffer, "HTTP/1.1 404 NOT FOUND\r\nConnection: close\r\n\r\n");
      return 2;
    }

    FILE *f;
    f = fopen(resolved_path, "r");

    if (f == NULL)
    {
      perror("fopen()");
      printf("File Not found\n");
      sprintf(response_buffer, "HTTP/1.1 404 NOT FOUND\r\nConnection: close\r\n\r\n");
      return 2;
    }

    fstat(fileno(f), &stats);

    if (response_buffer == NULL)
    {
      fprintf(stderr, "couldn't allocate memory for file buffer\n");
      sprintf(response_buffer, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nConnection: close\r\n\r\n");
      return 1;
    }

    size_t bytesread;

    do
    {
      bytesread = fread(response_buffer, sizeof(char), stats.st_size / sizeof(char), f);
    } while (bytesread > 0);

    fclose(f);

    return 0;
  }
  // if we handle json
  sprintf(response_buffer, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nConnection: close\r\n\r\n");
  return 1;
  // users has some generic json type data that can be returned
  // foo might have some long request line to practice stuff maybe
  // handle endpoint requests, e.g. stuff for json
}
