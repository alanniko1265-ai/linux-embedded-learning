#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<signal.h>
typedef struct{
    char monitor_path[128];
    char log_path[128];
    int interval_sec;
}MonitorConfig;
static volatile sig_atomic_t g_stop=0;
static void handle_sigint(int signo){
        (void)signo;
         g_stop=1;
}
static int load_config(const char *path,MonitorConfig *config){
    FILE *fp;
    char line[256];
    fp=fopen(path,"r");
    if(fp==NULL){
        perror("fopen config");
        return -1;
    }
    while(fgets(line,sizeof(line),fp)!=NULL){
        if(strncmp(line,"monitor_path=",13)==0){
            sscanf(line,"monitor_path=%127s",config->monitor_path);
        }

        else if(strncmp(line,"log_path=",9)==0){
            sscanf(line,"log_path=%127s",config->log_path);
        }
        else if(strncmp(line, "interval_sec=", 13) == 0){
            config->interval_sec=atoi(line+13);
        }
    }
    fclose(fp);
    return 0;
}
static int get_file_size(const char *path,long *size){
    struct  stat st;
    if(stat(path,&st)<0)
    {
        perror("stat monitor file");
        return -1;
    }
    *size=st.st_size;
    return 0;
}
int main(int argc,char *argv[]){
    MonitorConfig config;
    FILE *log_file;
    const char *config_path;
    long file_size;
    long last_size;
    long current_size;
    struct sigaction sa;
    if(argc<2){
        printf("usage: %s <config_path>\n",argv[0]);
        return 1;
    }
    config_path=argv[1];
    printf("config_path: %s\n",config_path);
    if(load_config(config_path,&config)<0){
        return 1;
    }
    printf("monitor_path: %s\n", config.monitor_path);
    printf("log_path: %s\n", config.log_path);
    printf("interval_sec: %d\n", config.interval_sec);
    if(get_file_size(config.monitor_path,&file_size)<0){
        return 1;
    }
    log_file=fopen(config.log_path,"a");
    if(log_file==NULL){
        perror("fopen log");
        return 1;
    }
    last_size=file_size;
    printf("initsize file size: %ld bytes\n",file_size);
    fprintf(log_file,"initsize file size: %ld bytes\n",file_size);
    fflush(log_file);
    sa.sa_handler =handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGINT,&sa,NULL)<0){
        perror("sigaction");
        fclose(log_file);
        return 1;
    }
    while(!g_stop){
        sleep(config.interval_sec);
        if(get_file_size(config.monitor_path,&file_size)<0){
            fclose(log_file);
            return 1;
        }
        current_size=file_size;
        if(current_size!=last_size){
            printf("file size changed: %ld -> %ld bytes\n",last_size,current_size);
            fprintf(log_file,"file size changed: %ld -> %ld bytes\n",last_size,current_size);
            fflush(log_file);
            last_size=current_size;
        }
        else{
            printf("file size unchanged: %ld bytes\n", current_size);
            fprintf(log_file,"file size unchanged: %ld bytes\n", current_size);
            fflush(log_file);
        }
    }
    printf("file monitor exit\n");
    fprintf(log_file,"file monitor exit\n");
    fflush(log_file);
    fclose(log_file);
    return 0;
}