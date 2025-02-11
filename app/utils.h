#ifndef UTILS_H_
#define UTILS_H_

int write_response_header(
  int status_code,
  long int content_length,
  char *response_header,
  char *content_type);

#endif
