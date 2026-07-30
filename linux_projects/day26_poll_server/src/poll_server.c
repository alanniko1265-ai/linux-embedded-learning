#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<poll.h>
#define SERVER_PORT 9000
#define BUFFER_SIZE 128
#define MAX_CLIENTS 16
#define POLL_FD_COUNT (MAX_CLIENTS + 1)
static int create_server_socket(void){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket");
        return -1;
    }
    int reuse=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        close(server_fd);
        perror("setsockopt");
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
    int server_fd=create_server_socket();
    if(server_fd<0){
        return 1;
    }
    struct pollfd fds[POLL_FD_COUNT];
    for(int i=0;i<POLL_FD_COUNT;i++){
        fds[i].events=POLLIN;
        fds[i].fd=-1;
        fds[i].revents=0;
    }
    fds[0].fd=server_fd;
        printf("poll tcp server listen on port %d\n", SERVER_PORT);
    while(1){
        int ready=poll(fds,POLL_FD_COUNT,-1);
        if(ready<0){
            perror("ready");
            break;
        }
        if(fds[0].revents & POLLIN){
            struct sockaddr_in client_addr;
            socklen_t client_len=sizeof(client_addr);
            int client_fd=accept(server_fd,(struct sockaddr *)&client_addr,&client_len);
            if(client_fd<0){
                perror("accept");
                continue;
            }
            int added=0;
            for(int i=1;i<POLL_FD_COUNT;i++){
                if(fds[i].fd==-1){
                    fds[i].fd=client_fd;
                    fds[i].events=POLLIN;
                    added=1;
                    break;
                }
            }
            if(!added){
                printf("too many clients");
                close(client_fd);
                continue;
            }
            printf("new client connected, fd=%d\n", client_fd);

        }
        for(int i=1;i<POLL_FD_COUNT;i++){
        int client_fd=fds[i].fd;
        if(!(fds[i].revents & POLLIN)){
            continue;
        }
        char buffer[BUFFER_SIZE];
        ssize_t n=recv(client_fd,buffer,sizeof(buffer)-1,0);
        if(n<0){
            perror("recv");
            fds[i].fd=-1;
            close(client_fd);
            continue;
        }
        if(n==0){
            printf("client disconnected, fd=%d\n",client_fd);
            fds[i].fd=-1;
            close(client_fd);
            continue;
        }
        buffer[n]='\0';
        printf("[fd=%d] recv: %s",client_fd,buffer);
        ssize_t sent=send(client_fd,buffer,n,0);
        if(sent<0){
            perror("send");
            close(client_fd);
            fds[i].fd=-1;
        }
    }
    }
    close(server_fd);
    return 0;
}