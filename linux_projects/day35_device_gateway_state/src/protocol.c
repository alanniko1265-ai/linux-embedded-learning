#include "protocol.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdint.h>
#include<sys/socket.h>
#include<arpa/inet.h>
ssize_t read_exact(int fd,void *buffer,size_t size){
    size_t total=0;
    char *ptr=buffer;
    while(total<size){
        ssize_t n=read(fd,ptr+total,size-total);
        if(n<0){
            perror("read");
            return -1;
        }
        else if(n==0){
            return 0;
        }
        total+=n;
    }
    return (ssize_t)total;
}
ssize_t send_exact(int fd,const void *buffer,size_t size){
    size_t total=0;
    const char *ptr=buffer;
    while(total<size){
        ssize_t n=send(fd,ptr+total,size-total,0);
        if(n<0){
            perror("send");
            return -1;
        }
        if(n==0){
            return -1;
        }
        total+=n;
    }
    return (ssize_t)total;
}
int read_message(int fd,char *buffer,size_t buffer_size){
    uint32_t net_len=0;
    ssize_t n=read_exact(fd,&net_len,sizeof(net_len));
    if(n<=0){
        return (int)n;
    }
    uint32_t len=ntohl(net_len);
    if(len==0||len>MAX_PAYLOAD_SIZE||len>=buffer_size){
        printf("invaild len of %u in read_message\n",len);
        return -1;
    }
    n=read_exact(fd,buffer,len);
    if(n<=0){
        return (int)n;
    }
    buffer[len]='\0';
    return (int)len;
}
int send_message(int fd,const char* message){
    uint32_t len=strlen(message);
    if(len==0||len>MAX_PAYLOAD_SIZE){
        printf("invaild len of %u in send message\n",len);
        return -1;
    }
    uint32_t net_len=htonl(len);
    ssize_t n=send_exact(fd,&net_len,sizeof(net_len));
    if(n!=sizeof(net_len)){
        return -1;
    }
    n=send_exact(fd,message,len);
    if(n!=len){
        return -1;
    }
    return 0;
}