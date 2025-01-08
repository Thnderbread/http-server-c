#ifndef HANDLEREQUEST_H_
#define HANDLEREQUEST_H_

/*Handle the http request*/
int handle_request(char *ept, char *method, char *http_ver, int *client_fd);

#endif
