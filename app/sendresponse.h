#ifndef SENDRESPONSE_H_
#define SENDRESPONSE_H_

int send_response(
  int request_status,
  int *client_fd,
  char *response_header,
  char *response_body,
  char *response_buffer);

#endif
