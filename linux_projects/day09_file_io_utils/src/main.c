#include "file_utils.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc,char *argv[]){
    int src_fd;
    int dst_fd;
    char buffer[128];
    if(argc<3){
        printf("usage:%s <src> <dst>\n",argv[0]);
        return 1;
    }
    src_fd=open(argv[1],O_RDONLY);
    if(src_fd<0){
        perror("open src");
        return 1;
    }
    dst_fd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(dst_fd<0){
        perror("open dst");
        close(src_fd);
        return 1;
    }
    ssize_t n;
    while((n=read(src_fd,buffer,sizeof(buffer)))>0){
        if(write_all(dst_fd,buffer,n)<0){
            perror("write_all");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
    }
    if(n<0){
        perror("read");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    close(src_fd);
    close(dst_fd);
    return 0;

}