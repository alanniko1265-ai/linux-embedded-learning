#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdint.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define MAX_PAYLOAD_SIZE 1024
static int connect_to_server(void);
static int send_message(int fd,const char *message);
static int send_message(int fd,const char *message){
    uint32_t length=strlen(message);
    if(length==0 || length>MAX_PAYLOAD_SIZE){
        printf("invalid messsage length: %u\n",length);
        return -1;
    }
    uint32_t net_length=htonl(length);
    ssize_t n=send(fd,&net_length,sizeof(net_length),0);
    if(n!=sizeof(net_length)){
        perror("send length");
        return -1;
    }
    n=send(fd,message,length,0);
    if(n!=length){
        perror("send payload");
        return -1;
    }
    return 0;
}
static int connect_to_server(void){
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0){
        perror("socket");
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
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
    int client_fd=connect_to_server();
    if(client_fd<0){
        return 1;
    }
    printf("connected to protocol server\n");
    char message[MAX_PAYLOAD_SIZE];
    while(1){
        if(fgets(message,sizeof(message),stdin)==NULL){
            break;
        }
        if(strcmp(message,"quit\n")==0){
            break;
        }
        if(send_message(client_fd,message)<0){
            break;
        }
    }
    close(client_fd);
    return 0;
}
