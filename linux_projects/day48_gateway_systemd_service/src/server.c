#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<signal.h>
#include<errno.h>
#include "command.h"
#include "protocol.h"
#include "logger.h"
#include "config.h"
#include "key_input.h"
#include "device_state.h"

#define BACKLOG 16
#define MAX_EVENTS 32
static volatile sig_atomic_t g_running=1;

static void handle_signal(int signo);
static int create_server_socket(int server_port);
static void handle_client_event(int epoll_fd,int client_fd);

static int create_server_socket(int server_port){
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        perror("socket failed in create_server_socket");
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
    server_addr.sin_port=htons(server_port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
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
static void handle_signal(int signo){
    (void)signo;
    g_running=0;
}
static void handle_client_event(int epoll_fd,int client_fd){
    char request[MAX_PAYLOAD_SIZE+1];
    char response[MAX_PAYLOAD_SIZE+1];
    int n=read_message(client_fd,request,sizeof(request));
    if(n<0){
        printf("read request failed,fd=%d\n",client_fd);
        logger_error("read request failed");
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
        close(client_fd);
        return;
    }
    else if(n==0){
        printf("client disconnect,fd=%d\n",client_fd);
        logger_info("client disconnected");
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }
    printf("[fd=%d] request: %s",client_fd,request);
    char log_message[MAX_PAYLOAD_SIZE+64];
    snprintf(log_message,sizeof(log_message),"request received: %s",request);
    logger_info(log_message);
    build_response(request,response,sizeof(response));
    snprintf(log_message, sizeof(log_message), "response built: %s", response);
    logger_info(log_message);
    if(send_message(client_fd,response)<0){
        printf("send reponse failed,fd=%d\n",client_fd);
        logger_error("send response failed");
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
        close(client_fd);
        return;
    }
}
int main(void){
    struct GatewayConfig config;
    signal(SIGINT,handle_signal);
    signal(SIGTERM,handle_signal);
    if(load_config("config/gateway.conf",&config)<0){
        return 1;
    }
    if(logger_init(config.log_path)<0){
        return 1;
    }
    logger_info("server starting");
    if(key_input_start(device_state_global())<0){
        logger_error("key input start failed");
    }
    int server_fd=create_server_socket(config.server_port);
    if(server_fd<0){
        logger_error("create server socket failed");
        logger_close();
        return 1;
    }
    int epoll_fd=epoll_create1(0);
    if(epoll_fd<0){
        perror("epoll create1");
        logger_error("epoll_create1 failed");
        close(server_fd);
        logger_close();
        return 1;
    }
    struct epoll_event event;
    memset(&event,0,sizeof(event));
    event.events=EPOLLIN;
    event.data.fd=server_fd;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_fd,&event)<0){
        perror("epoll_ctl add server_fd");
        logger_error("epoll_ctl add server_fd failed");
        close(server_fd);
        close(epoll_fd);
        logger_close();
        return 1;
    }
    printf("%s server listening on port %d\n",config.device_name,config.server_port);
    logger_info("server config loaded");
    struct epoll_event events[MAX_EVENTS];
    while(g_running){
        int ready=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if(ready<0){
            if(errno==EINTR){
                continue;
            }
            perror("epoll_wait");
            break;
        }
        for(int i=0;i<ready;i++){
            if(events[i].data.fd==server_fd){
                struct sockaddr_in client_addr;
                socklen_t client_len=sizeof(client_addr);
                int client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&client_len);
                if(client_fd<0){
                    perror("accept");
                    continue;
                }
                printf("client connected, fd=%d\n", client_fd);
                logger_info("client connected");
                struct epoll_event client_event;
                memset(&client_event,0,sizeof(client_event));
                client_event.events=EPOLLIN;
                client_event.data.fd=client_fd;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&client_event)<0){
                    perror("epoll_ctl add client_fd");
                    close(client_fd);
                    continue;
                }
            }
            else{
                handle_client_event(epoll_fd,events[i].data.fd);
            }
        }
    }
    logger_info("server stopped");
    close(epoll_fd);
    close(server_fd);
    logger_close();
    return 0;
}