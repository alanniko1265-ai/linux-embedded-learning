#ifndef COMMAND_H
#define COMMAND_H

#include<stddef.h>

#define RESPONSE_OK 0
#define RESPONSE_ERR_UNKNOWN_COMMAND 1001

void build_response(const char *request,char *response,size_t response_size);

#endif