#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define MAX_PAYLOAD_SIZE 1024
static int connect_to_server(void);
static ssize_t read_exact(int fd,void *buffer,size_t size);
static ssize_t send_exact(int fd,const void *buffer,size_t size);
static int read_message(int fd,char *buffer,size_t buffer_size);
static int send_message(int fd,const char *message);
static ssize_t read_exact(int fd,void *buffer,size_t size){
    size_t total=0;
    char *ptr=buffer;
    while(total<size){
        ssize_t n=recv(fd,ptr+total,size-total,0);
        if(n<0){
            perror("recv");
            return -1;
        }
        if(n==0){
            return 0;
        }
        total+=n;
    }
    return (ssize_t)total;
}
static ssize_t send_exact(int fd,const void *buffer,size_t size){
    size_t total=0;
    const char *ptr=buffer;
    while(total<size){
        ssize_t n=send(fd,ptr+total,size-total,0);
        if(n<0){
            perror("send");
            return -1;
        }
        else if(n==0){
            return -1;
        }
        total+=n;
    }
    return (ssize_t)total;
}
static int read_message(int fd,char *buffer,size_t buffer_size){
    uint32_t net_len=0;
    ssize_t n=read_exact(fd,&net_len,sizeof(net_len));
    if(n<=0){
        return (int)n;
    }
    uint32_t len=ntohl(net_len);
    if(len==0||len>MAX_PAYLOAD_SIZE||len>=buffer_size){
        printf("invalid message length: %u\n",len);
        return -1;
    }
    n=read_exact(fd,buffer,len);
    if(n<=0){
        return (int)n;
    }
    buffer[len]='\0';
    return (int)len;
}
static int send_message(int fd,const char *message){
    uint32_t len=strlen(message);
    if(len==0||len>MAX_PAYLOAD_SIZE){
        printf("invalid message length: %u\n",len);
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
    printf("connected to request-response server\n");
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
        int n=send_message(client_fd,request);
        if(n<0){
            printf("send failed\n");
            break;
        }
        n=read_message(client_fd,response,sizeof(response));
        if(n<0){
            printf("read failed");
            break;
        }
        if(n==0){
            printf("server disconnected");
            break;
        }
        printf("[response] %s\n",response);
    }
    close(client_fd);

    return 0;
}