#include "protocol.h"
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include "config.h"
#define SERVER_IP "127.0.0.1"

static int connect_to_server(int server_port);

static int connect_to_server(int server_port){
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0){
        perror("socket of client");
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(server_port);
    if(inet_pton(AF_INET,SERVER_IP,&server_addr.sin_addr)<=0){
        perror("inet_pton");
        close(client_fd);
        return -1;
    }
    if(connect(client_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        perror("connect");
        close(client_fd);
        return -1;
    }
    return client_fd;
}
int main(void){
    struct GatewayConfig config;
    if(load_config("config/gateway.conf",&config)<0){
        return 1;
    }
    int client_fd=connect_to_server(config.server_port);
    if(client_fd<0){
        return 1;
    }
    char request[MAX_PAYLOAD_SIZE+1];
    char response[MAX_PAYLOAD_SIZE+1];
    while(1){
        printf("> ");
        if(fgets(request,sizeof(request),stdin)==NULL){
            break;
        }
        if(strcmp(request,"quit\n")==0){
            break;
        }
        if(send_message(client_fd,request)<0){
            printf("send message failed\n");
            break;
        }
        int n=read_message(client_fd,response,sizeof(response));
        if(n<0){
            printf("read message failed\n");
            break;
        }
        else if(n==0){
            printf("server disconnect\n");
            break;
        }
        printf("[response] %s\n",response);
    }
    close(client_fd);
    return 0;
}