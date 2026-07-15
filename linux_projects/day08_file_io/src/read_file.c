#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
int main(int argc,char *argv[]){
    int fd;
    char buffer[128];
    ssize_t n;
    if(argc<2){
        printf("usage :%s <file>\n",argv[0]);
        return 1;
    }
    fd=open(argv[1],O_RDONLY);
    if(fd<0){
        perror("open");
        return 1;
    }
    while((n=read(fd,buffer,sizeof(buffer)))>0){
        write(STDOUT_FILENO,buffer,n);
    }
    if(n<0){
        perror("read");
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}