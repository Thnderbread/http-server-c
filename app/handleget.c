#include "handleget.h"
#include "cJSON.h"
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

int handle_json_request(
  char *ept,
  char *cwd,
  int *http_status_code,
  char *content_type,
  char *response_body) {
  // tokenize url endpoint to determine requested resource
  char *tokens[5];
  char resource_copy[strlen(ept) + 1]; // for strtok
  strcpy(resource_copy, ept);

  int count = 0; // only 3 tokens (api, foo, and the resource id)
  char *line = strtok(resource_copy, "/");

  while (line != NULL) {
    tokens[count] = line;

    line = strtok(NULL, "/");
    count++;
  }

  // reconstruct arr as a path w/ path seps
  char db_path[PATH_MAX + 1];
  snprintf(
    db_path,
    sizeof(db_path),
    "%s%cdb%c%s.json",
    cwd,
    kPathSeparator,
    kPathSeparator,
    tokens[1]);

  char resolved_path[sizeof(db_path)];
  if (realpath(db_path, resolved_path) == NULL) {
    printf("Created path: %s | resolved path: %s\n", db_path, resolved_path);
    perror("realpath() - db");
    *http_status_code = 400;
    return 2;
  }

  // Read json data
  FILE *f;
  f = fopen(resolved_path, "r");

  if (f == NULL) {
    perror("fopen()");
    *http_status_code = 404;
    return 2;
  }

  struct stat stats;
  fstat(fileno(f), &stats);

  char json_buffer[1024];
  int len = fread(json_buffer, 1, sizeof(json_buffer), f);
  fclose(f);

  // Parse raw json output from file
  cJSON *json = cJSON_Parse(json_buffer);
  if (json == NULL) {
    const char *error = cJSON_GetErrorPtr();
    if (error != NULL) {
      fprintf(stderr, "JSON Parse error: %s\n", error);
    }
    *http_status_code = 500;

    cJSON_Delete(json);
    return 1;
  }

  // ? curl -v http://localhost:1337/api/users/1234 --output -
  cJSON *user = NULL;
  user = cJSON_GetObjectItem(json, tokens[2]); // tokens[2] holds ept id
  char *user_json = cJSON_PrintUnformatted(user);

  if (user_json == NULL) {
    cJSON_Delete(json);
    *http_status_code = 404;
    return 2;
  }

  // write json to response body
  snprintf(response_body, strlen(user_json) + 1, "%s", user_json);
  cJSON_free(user_json);
  cJSON_Delete(json);

  snprintf(
    content_type,
    strlen("Content-Type: application/json") + 1,
    "Content-Type: application/json");
  *http_status_code = 200;
  return 0;
}

int handle_static_request(
  char *ept,
  char *cwd,
  int *http_status_code,
  char *content_type,
  char *response_body) {
  // handle static file requests
  // request index page if requested path is '/'
  const char *resource = strcmp(ept, "/") == 0 ? "/index.html" : ept;

  // construct path to dist dir
  const char *dist_dirname = "dist";
  char dist_dir[PATH_MAX + 1];
  snprintf(
    dist_dir,
    PATH_MAX + 1,
    "%s%cdist%s",
    cwd,
    kPathSeparator,
    resource); // ept_dupe has a '/' at the front of it

  // look for the file to serve
  char resolved_path[PATH_MAX];
  if (realpath(dist_dir, resolved_path) == NULL) {
    perror("realpath() - dist");
    *http_status_code = 404;
    return 2;
  }

  FILE *f;
  f = fopen(resolved_path, "r");

  if (f == NULL) {
    perror("fopen()");
    *http_status_code = 404;
    return 2;
  }

  struct stat stats;
  fstat(fileno(f), &stats);

  size_t bytesread;
  do {
    bytesread =
      fread(response_body, sizeof(char), stats.st_size / sizeof(char) + 1, f);
  } while (bytesread > 0);

  fclose(f);
  snprintf(
    content_type,
    strlen("Content-Type: text/html; charset=utf-8") / sizeof(char) + 1,
    "Content-Type: text/html; charset=utf-8");
  *http_status_code = 200;
  return 0;
}
// Handle the http request
// return 0 if success
// return 1 if some error happened, 500 class http error
// return 2 if something like 404 is being returned
int handle_get(
  char *ept,
  char *content_type,
  char *response_body,
  int *http_status_code,
  int response_body_size) {
  printf("GET Handler triggered, requested endpoint: %s\n", ept);

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd()");
    *http_status_code = 500;
    return 1;
  }

  if (strstr(ept, "/api/")) {
    return handle_json_request(
      ept, cwd, http_status_code, content_type, response_body);
  }

  return handle_static_request(
    ept, cwd, http_status_code, content_type, response_body);
}
