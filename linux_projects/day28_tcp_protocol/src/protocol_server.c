#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define SERVER_PORT 9000
#define BACKLOG 16
#define MAX_PAYLOAD_SIZE 1024
static int create_server_socket(void);
static ssize_t read_exact(int fd,void *buffer,size_t size);
static int read_message(int fd,char *buffer,size_t buffer_size);
static ssize_t read_exact(int fd,void *buffer,size_t size){
    size_t total=0;
    char *ptr=buffer;
    while(total<size){
        ssize_t n=recv(fd,ptr+total,size-total,0);
        if(n<0){
            perror("recv");
            return -1;
        }
        else if(n==0){
            return 0;
        }
        total+=n;
    }
    return (ssize_t)total;
}
static int read_message(int fd,char *buffer,size_t buffer_size){
    uint32_t net_lenth=0;
    ssize_t n=read_exact(fd,&net_lenth,sizeof(net_lenth));
    if(n<=0){
        return (int)n;
    }
    uint32_t length=ntohl(net_lenth);
    if(length==0 || length>MAX_PAYLOAD_SIZE || length>=buffer_size){
        printf("invalid message length: %u\n",length);
        return -1;
    }
    n=read_exact(fd,buffer,length);
    if(n<=0){
        return (int)n;
    }
    buffer[length]='\0';
    return (int)length;
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
    if(listen(server_fd,BACKLOG)<0){
        perror("listen");
        close(server_fd);
        return -1;
    }
    return server_fd;
}
int main(void){
    int server_fd=create_server_socket();
    if(server_fd<0){
        perror("create");
        return 1;
    }
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);
    int client_fd=accept(server_fd,(struct sockaddr *)&client_addr,&client_len);
    if(client_fd<0){
        perror("accept");
        return -1;
    }
    printf("client connected\n");
    char message[MAX_PAYLOAD_SIZE+1];
    while(1){
        ssize_t n=read_message(client_fd,message,sizeof(message));
        if(n<0){
            printf("read message failed\n");
            break;
        }
        if(n==0){
            printf("client disconnected\n");
            break;
        }
        printf("[message] len=%d, data=%s\n",(int)n,message);
    }
    close(client_fd);
    close(server_fd);
    return 0;
}