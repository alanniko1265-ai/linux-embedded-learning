#ifndef PROTOCOL_H
#define PROTOCOL_H

#include<stddef.h>
#include<sys/types.h>

#define MAX_PAYLOAD_SIZE 1024

ssize_t read_exact(int fd,void *buffer,size_t size);
ssize_t send_exact(int fd,const void *buffer,size_t size);
int read_message(int fd,char *buffer,size_t buffer_size);
int send_message(int fd,const char *message);

#endif 