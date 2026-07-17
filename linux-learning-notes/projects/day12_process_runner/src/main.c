#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
int main(int argc,char *argv[]){
    pid_t pid;
    int status;
    if(argc<2){
        printf("usage: %s <program> [args...]\n",argv[0]);
        return 1;
    }
    pid=fork();
    if(pid<0){
        perror("fork");
        return 1;
    }
    if(pid==0){
        execvp(argv[1],&argv[1]);
        perror("execvp");
        return 1;
    }
    else{
        printf("parent process,child pid = %d\n",pid);
        if(waitpid(pid,&status,0)<0){
            perror("waitpid");
            return 1;
        }
        printf("child finished\n");
        if(WIFEXITED(status)){
            printf("child exit code=%d\n",WEXITSTATUS(status));
        }
        else{
            printf("child did not exit normally\n");
        }
    }
    return 0;
}