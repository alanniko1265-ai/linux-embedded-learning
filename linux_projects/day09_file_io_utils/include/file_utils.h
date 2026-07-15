#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <sys/types.h>
ssize_t write_all(int fd,const void *buffer,size_t count);
#endif