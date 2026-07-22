#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#define FIFO_PATH "/tmp/day21_device_fifo"
int main(void){
    if(mkfifo(FIFO_PATH,0666)<0){
        if(errno!=EEXIST){
            perror("mkfifo");
            return 1;
        }
    }
    int fd=open(FIFO_PATH,O_RDONLY);
    if(fd<0){
        perror("open fifo reader");
        return 1;
    }
    char buffer[128];
    while(1){
        ssize_t n=read(fd,buffer,sizeof(buffer)-1);
        if(n<0){
            perror("read fifo");
            close(fd);
            return 1;
        }
        if(n==0){
            break;
        }
        buffer[n]='\0';
        printf("[reader] %s",buffer);
    }
    close(fd);
    return 0;
}