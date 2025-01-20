#ifndef HANDLEGET_H_
#define HANDLEGET_H_

/*Handle the http request
return 0 if success
return 1 if some error happened, 500 class http error
return 2 if something like 404 is being returned
*/
int handle_get(char *ept, char *response_buffer);

#endif
