#include<stdio.h>
#include<unistd.h>
#include<signal.h>
static volatile sig_atomic_t g_stop=0;
static void handle_sigint(int signo){
    (void)signo;
    g_stop=1;
}
int main(void){
    FILE *log_file;
    int count=0;
    struct sigaction sa;
    sa.sa_handler=handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGINT,&sa,NULL)<0){
        perror("sigaction");
        return 1;
    }
    log_file=fopen("signal_guard.log","a");
    if(log_file==NULL){
        perror("fopen");
        return 1;
    }
    while(!g_stop){
        printf("running...%d\n",count);
        fprintf(log_file,"running...%d\n",count);
        fflush(log_file);
        count++;
        sleep(1);
    }
    if(g_stop){
        printf("\nSIGINT received, stopping...\n");
    }
    printf("clean exit\n");
    fclose(log_file);
    return 0;
}
