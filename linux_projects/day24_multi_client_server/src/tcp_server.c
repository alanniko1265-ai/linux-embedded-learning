#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<pthread.h>
#define SERVER_PORT 9000
#define BUFFER_SIZE 128
struct ClientInfo{
    int client_fd;
    struct sockaddr_in client_addr;
};
static void *client_thread(void *arg){
    struct ClientInfo *client=(struct ClientInfo*)arg;
    int client_fd=client->client_fd;
    char client_ip[INET_ADDRSTRLEN];
    int client_prot=ntohs(client->client_addr.sin_port);
    inet_ntop(AF_INET,&client->client_addr.sin_addr,client_ip,sizeof(client_ip));
    printf("client connected: %s:%d\n",client_ip,client_prot);
    char buffer[BUFFER_SIZE];
    while(1){
        ssize_t n=recv(client_fd,buffer,sizeof(buffer)-1,0);
        if(n<0){
            perror("recv");
            break;
        }
        if(n==0){
            printf("client disconnected %s:%d",client_ip,client_prot);
            break;
        }
        buffer[n]='\0';
        printf("[client %s:%d] %s",client_ip,client_prot,buffer);
        ssize_t sent=send(client_fd,buffer,n,0);
        if(sent<0){
            perror("sent");
            break;
        }
    }
    close(client_fd);
    free(client);
    return NULL;
}
int main(void){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket");
        return 1;
    }
    int reuse=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        perror("setsockopt");
        close(server_fd);
        return 1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
        perror("bind");
        close(server_fd);
        return 1;
    }
    if(listen(server_fd,10)<0){
        perror("listen");
        close(server_fd);
        return 1;
    }
    printf("multi-client tcp sever listen on port %d\n",SERVER_PORT);
    while(1){
        struct ClientInfo *client=malloc(sizeof(struct ClientInfo));
        if(client==NULL){
            perror("malloc");
            continue;
        }
        socklen_t client_len=sizeof(client->client_addr);
        client->client_fd=accept(server_fd,(struct sockaddr *)&client->client_addr,&client_len);
        if(client->client_fd<0){
            perror("accept");
            free(client);
            continue;
        }
        pthread_t tid;
        if(pthread_create(&tid,NULL,client_thread,client)!=0){
            perror("create");
            close(client->client_fd);
            free(client);
            continue;
        }
        pthread_detach(tid);
    }
    close(server_fd);
    return 0;
}