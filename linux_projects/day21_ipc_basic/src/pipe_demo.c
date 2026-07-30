#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
int main(void){
    int pipe_fd[2];
    if(pipe(pipe_fd)<0){
        perror("pipe");
        return 1;
    }
    pid_t pid=fork();
    if(pid<0){
        perror("fork");
        return 1;
    }
    if(pid==0){
        close(pipe_fd[0]);
        const char *message[]={
            "device data: temperature=32\n",
            "device data: humidity=60\n",
            "device data: voltage=24\n"
        };
        for(int i=0;i<3;i++){
            write(pipe_fd[1],message[i],strlen(message[i]));
        }
        close(pipe_fd[1]);
        return 0;
    }
    char buffer[128];
    close(pipe_fd[1]);
    while(1){
        ssize_t n=read(pipe_fd[0],buffer,sizeof(buffer)-1);
        if(n<0){
            perror("read");
            close(pipe_fd[0]);
            waitpid(pid,NULL,0);
            return 1;
         }
         if(n==0){
            break;
         }
         buffer[n]='\0';
        printf("[parent] receive: %s",buffer);
    }
    close(pipe_fd[0]);
    waitpid(pid,NULL,0);
    return 0;
}