#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdint.h>
#include<sys/epoll.h>
#define SERVER_PORT 9000
#define BACKLOG 16
#define MAX_EVENTS 32
#define MAX_PAYLOAD_SIZE 1024
static int create_server_socket(void);
static ssize_t read_exact(int fd,void *buffer,size_t size);
static ssize_t send_exact(int fd,const void *buffer,size_t size);
static int read_message(int fd,char *buffer,size_t buffer_size);
static int send_message(int fd,const char *message);
static void build_response(const char *request,char *response,size_t response_size);
static void handle_client_event(int epoll_fd,int client_fd);
static ssize_t read_exact(int fd,void *buffer,size_t size){
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
static ssize_t send_exact(int fd,const void* buffer,size_t size){
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
static int  read_message(int fd,char *buffer,size_t buffer_size){
    uint32_t net_len=0;
    ssize_t n=read_exact(fd,&net_len,sizeof(net_len));
    if(n<=0){
        return (int)n;
    }
    uint32_t len=ntohl(net_len);
    if(len==0||len>MAX_PAYLOAD_SIZE||len>=buffer_size){
        printf("invaild size of len %u from read\n",len);
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
        printf("invaild size of len %u from send\n",len);
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
static void build_response(const char *request,char *response,size_t response_size){
    if(strcmp(request,"status\n")==0){
        snprintf(response,response_size,"OK device=online temp=32 voltage=24");
    }
    else if(strcmp(request,"led on\n")==0){
        snprintf(response,response_size,"OK led=on");
    }
    else if(strcmp(request,"led off\n")==0){
        snprintf(response,response_size,"OK led=off");
    }
    else if(strcmp(request,"reboot\n")==0){
        snprintf(response,response_size,"OK reboot scheduled");
    }
    else{
        snprintf(response, response_size, "ERR unknown command");
    }
}
static void handle_client_event(int epoll_fd,int client_fd){
    char request[MAX_PAYLOAD_SIZE+1];
    char response[MAX_PAYLOAD_SIZE+1];
    ssize_t n=read_message(client_fd,request,sizeof(request));
    if(n<0){
        printf("read request failed fd=%d\n",client_fd);
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
        close(client_fd);
        return;
    }
    if(n==0){
        printf("client disconnected,fd=%d\n",client_fd);
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
        close(client_fd);
        return;
    }
    printf("[fd=%d] request: %s",client_fd,request);
    build_response(request,response,sizeof(response));
    if(send_message(client_fd,response)<0){
        perror("send");
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
        close(client_fd);
        return;
    }
}
static int create_server_socket(void){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket");
        return -1;
    }
    int reuse=1;
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        perror("setsocketopt");
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
        return 1;
    }
    int epoll_fd=epoll_create1(0);
    if(epoll_fd<0){
        perror("epoll_create");
        close(server_fd);
        return 1;
    }
    struct epoll_event event;
    memset(&event,0,sizeof(event));
    event.data.fd=server_fd;
    event.events=EPOLLIN;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_fd,&event)<0){
        perror("epoll_ctl and server_fd");
        close(server_fd);
        close(epoll_fd);
        return 1;
    }
    printf("epoll protocol server listening on port %d\n", SERVER_PORT);
    struct epoll_event events[MAX_EVENTS];
    while(1){
        int ready=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if(ready<0){
            perror("epoll_wait");
            break;
        }
        for(int i=0;i<ready;i++){
            int fd=events[i].data.fd;
            if(fd==server_fd){
                 struct sockaddr_in client_addr;
                 socklen_t client_len=sizeof(client_addr);
                 int client_fd=accept(fd,(struct sockaddr*)&client_addr,&client_len);
                 if(client_fd<0){
                    perror("accept");
                    continue;
                 }
                 printf("client connected, fd=%d\n", client_fd);
                 struct epoll_event client_event;
                 memset(&client_event,0,sizeof(client_event));
                 client_event.data.fd=client_fd;
                 client_event.events=EPOLLIN;
                 if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&client_event)<0){
                    perror("epoll_ctl add client_fd");
                    close(client_fd);
                    continue;
                 }
            }
            else{
                handle_client_event(epoll_fd,fd);
            }
        }
    }
    close(server_fd);
    close(epoll_fd);
    return 0;
}