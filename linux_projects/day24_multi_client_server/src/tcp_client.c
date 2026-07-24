#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 128
int main(int argc,char *argv[]){
    if(argc<2){
        printf("usage: %s <message>\n",argv[0]);
        return 1;
    }
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0){
        perror("socket");
        return 1;
    }
    struct sockaddr_in sever_addr;
    memset(&sever_addr,0,sizeof(sever_addr));
    sever_addr.sin_family=AF_INET;
    sever_addr.sin_port=htons(SERVER_PORT);
    if(inet_pton(AF_INET,SERVER_IP,&sever_addr.sin_addr)<=0){
        perror("inet_pton");
        close(client_fd);
        return 1;
    }
    if(connect(client_fd,(struct sockaddr *)&sever_addr,sizeof(sever_addr))<0){
        perror("connect");
        close(client_fd);
        return 1;
    }
    char send_buffer[BUFFER_SIZE];
    send_buffer[0]='\0';
    for(int i=1;i<argc;i++){
        strncat(send_buffer,argv[i],sizeof(send_buffer)-strlen(send_buffer)-1);
        if(i!=argc-1){
            strncat(send_buffer," ",sizeof(send_buffer)-strlen(send_buffer)-1);
        }
    }
    strncat(send_buffer,"\n",sizeof(send_buffer)-strlen(send_buffer)-1);
    ssize_t sent=send(client_fd,send_buffer,strlen(send_buffer),0);
    if(sent<0){
        perror("send");
        close(client_fd);
        return 1;
    }
    char buffer[BUFFER_SIZE];
    ssize_t n=recv(client_fd,buffer,sizeof(buffer)-1,0);
    if(n<0){
        perror("recv");
        close(client_fd);
        return 1;
    }
    if(n==0){
        printf("server disconnected\n");
        close(client_fd);
        return 1;
    }
    buffer[n]='\0';
    printf("[clinent] echo %s",buffer);
    close(client_fd);
    return 0;
}