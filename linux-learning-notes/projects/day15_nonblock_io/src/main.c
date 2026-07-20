#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
int  main(void){
    int count=0;
    int flags;
    char buffer[128];
    ssize_t n;
    flags=fcntl(STDIN_FILENO,F_GETFL);
    if(flags<0){
        perror("fcntl F_GETFL");
        return 1;
    }
    if(fcntl(STDIN_FILENO,F_SETFL,flags| O_NONBLOCK)<0){
        perror("fcntl F_SETFL");
        return 1;
    }
    while(1){
        printf("waiting input...\n");
        n=read(STDIN_FILENO,buffer,sizeof(buffer)-1);
        if(n>0){
            buffer[n]='\0';
            printf("input: %s",buffer);
            if(strncmp(buffer,"quit",4)==0){
                printf("quit command received\n");
                break;
            }
        }
        else if(n<0){
            if(errno==EAGAIN || errno==EWOULDBLOCK){
                printf("no input\n");
            }
            else{
                perror("read");
                return 1;
            }
        }
        printf("loop %d\n",count);
        count++;
        sleep(1);
    }





    return 0;
}
