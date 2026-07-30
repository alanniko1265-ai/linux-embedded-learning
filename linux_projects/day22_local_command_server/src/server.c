#include<stdio.h>
#include<time.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#define FIFO_PATH "/tmp/day22_command_fifo"
#define LOG_PATH "logs/server.log" 
static volatile sig_atomic_t g_running=1;
static void handle_signal(int signal){
    (void)signal;
    g_running=0;
}
static void write_log(FILE *log_file,const char *message){
    time_t now=time(NULL);
    struct tm *local=localtime(&now);
    if(local==NULL){
        fprintf(log_file,"unknown time %s\n",message);
        fflush(log_file);
        return ;
    }
    fprintf(log_file,"[%04d-%02d-%02d %02d-%2d-%2d] %s\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec,message);
    fflush(log_file);
}
int main(void){
    signal(SIGINT,handle_signal);
    signal(SIGTERM,handle_signal);
    if(mkfifo(FIFO_PATH,0666)<0){
        if(errno!=EEXIST){
            perror("mkfifo");
            return 1;
        }
    }
    FILE *log_file=fopen(LOG_PATH,"a");
    if(log_file==NULL){
        perror("open log_file");
        return 1;
    }
    int read_fd=open(FIFO_PATH,O_RDONLY);
    int keep_fd=open(FIFO_PATH,O_WRONLY);
    if(read_fd<0){
        perror("open fifo read");
        fclose(log_file);
        return 1;
    }
    if(keep_fd<0){
        perror("open fifo keep");
        fclose(log_file);
        close(read_fd);
        return 1;
    }
    write_log(log_file,"sever start");
    char buffer[128];
    while(g_running){
        ssize_t n=read(read_fd,buffer,sizeof(buffer)-1);
        if(n<0){
            if(errno==EINTR){
                continue;;
            }
            perror("read");
            close(read_fd);
            fclose(log_file);
            return 1;
        }
        if(n==0){
            break;
        }
        buffer[n]='\0';
        printf("[server] command %s",buffer);
        write_log(log_file,buffer);
    }
    write_log(log_file, "server exit");
    close(read_fd);
    close(keep_fd);
    fclose(log_file);
    return 0;
}
