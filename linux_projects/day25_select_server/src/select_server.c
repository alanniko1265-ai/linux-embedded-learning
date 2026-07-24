#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/select.h>
#define SERVER_PORT 9000
#define BUFFER_SIZE 128
#define MAX_CLIENTS 16
static void init_clients(int clients[],int size){
    for(int i=0;i<size;i++){
        clients[i]=-1;
    }
}
static int add_client(int clients[],int size,int client_fd){
    for(int i=0;i<size;i++){
        if(clients[i]==-1){
            clients[i]=client_fd;
              return 0;
        }
    }
    return -1;
}
static void remove_client(int clients[],int index){
    close(clients[index]);
    clients[index]=-1;
}
static int create_server_socket(void){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket");
        return -1;
    }
    int reuse=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        perror("bind");
        close(server_fd);
        return -1;
    }
    if(listen(server_fd,10)<0){
        perror("listen");
        close(server_fd);
        return -1;
    }
    return server_fd;
}
int main(void){
    int clients[MAX_CLIENTS];
    init_clients(clients,MAX_CLIENTS);
    int server_fd=create_server_socket();
    if(server_fd<0){
        return 1;
    }
    printf("select tcp server listen on port %d\n", SERVER_PORT);
    while(1){
        fd_set read_fps;
        FD_ZERO(&read_fps);
        FD_SET(server_fd,&read_fps);
        int max_fd=server_fd;
        for(int i=0;i<MAX_CLIENTS;i++){
            if(clients[i]!=-1){
                FD_SET(clients[i],&read_fps);
                if(clients[i]>max_fd){
                    max_fd=clients[i];
                }
            }
        }
        int ready=select(max_fd+1,&read_fps,NULL,NULL,NULL);
        if(ready<0){
            perror("ready");
            close(server_fd);
            return 1;
        }
        if(FD_ISSET(server_fd,&read_fps)){
            struct sockaddr_in client_addr;
            socklen_t client_len=sizeof(client_addr);
            int client_fd=accept(server_fd,(struct sockaddr *)&client_addr,&client_len);
            if(client_fd<0){
                perror("accept");
                continue;
            }
            if(add_client(clients,MAX_CLIENTS,client_fd)<0){
                printf("too many clients\n");
                close(client_fd);
                continue;
            }
            printf("new client connected, fd=%d\n",client_fd);
        }
        for(int i=0;i<MAX_CLIENTS;i++){
            int client_fd=clients[i];
            if(client_fd==-1){
                continue;
            }
            if (!FD_ISSET(client_fd, &read_fps)) {
                continue;
            }
            char buffer[BUFFER_SIZE];
            ssize_t n=recv(client_fd,buffer,sizeof(buffer)-1,0);
            if(n<0){
                perror("recv");
                remove_client(clients,i);
                continue;
            }
            if(n==0){
                printf("client disconnected, fd=%d\n",client_fd);
                remove_client(clients,i);
                continue;
            }
            buffer[n]='\0';
            printf("[fd=%d] recv: %s",client_fd,buffer);
            ssize_t sent;
            sent=send(client_fd,buffer,n,0);
            if(sent<0){
                perror("send");
                remove_client(clients,i);
            }

        }
    }
    close(server_fd);
    return 0;
}