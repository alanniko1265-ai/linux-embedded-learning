#include"file_utils.h"
#include<unistd.h>
ssize_t write_all(int fd,const void *buffer,size_t count){
    const char *p=buffer;
    size_t total=0;
    while(total<count){
        ssize_t written;
        written=write(fd,p+total,count-total);
        if(written<0){
            return -1;
        }
        total+=written;
    }
    return (ssize_t)total;
}