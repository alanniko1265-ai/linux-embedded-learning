#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#define FIFO_PATH "/tmp/day22_command_fifo"
int main(int argc,char *argv[]){
    if(argc<2){
        printf("usage %s <command>\n",argv[0]);
        return 1;
    }
    int fd=open(FIFO_PATH,O_WRONLY);
    if(fd<0){
        perror("open fifo");
        return 1;
    }
    for(int i=1;i<argc;i++){
        ssize_t n=write(fd,argv[i],strlen(argv[i]));
        if(n<0){
            perror("write");
            close(fd);
            return 1;
        }
        if(i!=argc-1){
            write(fd," ",1);;
        }
    }
    write(fd, "\n", 1);
    close(fd);
    return 0;
}