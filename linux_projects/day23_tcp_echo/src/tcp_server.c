#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SERVER_PORT 9000
#define BUFFER_SIZE 128
int main(void){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket");
        return 1;
    }
    int reuse=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        perror("setsockppt");
        close(server_fd);
        return 1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        perror("bind");
        close(server_fd);
        return 1;
    }
    if(listen(server_fd,5)<0){
        perror("listen");
        close(server_fd);
        return 1;
    }
    printf("tcp server listen on port%d\n",SERVER_PORT);
    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_len=sizeof(client_addr);
        int client_fd=accept(server_fd,(struct sockaddr *)&client_addr,&client_len);
        if(client_fd<0){
            perror("accept");
            close(server_fd);
            return 1;
        }
        printf("client connected\n");
        char buffer[BUFFER_SIZE];
        while(1){
            ssize_t n=recv(client_fd,buffer,sizeof(buffer)-1,0);
            if(n<0){
                perror("recv");
                close(server_fd);
                close(client_fd);
                return 1;
            }
            if(n==0){
                  printf("client disconnected\n");
                break;
            }
            buffer[n]='\0';
            printf("[sever] recv: %s\n",buffer);
            ssize_t sent=send(client_fd,buffer,n,0);
            if(sent<0){
                perror("send");
                close(client_fd);
                close(server_fd);
                return 1;
            }
        }
        close(client_fd);
    }
    close(server_fd);
    return 0;
}