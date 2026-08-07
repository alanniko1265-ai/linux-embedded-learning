#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<limits.h>

#define LED_BASE_PATH "/sys/class/leds"

int main(int argc,char *argv[]){
    if(argc!=3){
        printf("usage: %s <red|green|blue> <on|off>\n",argv[0]);
        return  1;
    }
    const char *led_name=argv[1];
    if(strcmp(led_name,"red")!=0 && strcmp(led_name,"green")!=0 && strcmp(led_name,"blue")!=0){
        printf("invalid led: %s\n",led_name);
        printf("support: red green blue\n");
        return 1;
    }
    const char *action=argv[2];
    const char *value=NULL;
    if(strcmp(action,"on")==0){
        value="1";
    }
    else if(strcmp(action,"off")==0){
        value="0";
    }
    else{
        printf("invalid action: %s\n",action);
        printf("support: on off\n");
        return 1;
    }
    char path[PATH_MAX];
    snprintf(path,sizeof(path),"%s/%s/brightness",LED_BASE_PATH,led_name);
    printf("write %s to %s\n",value,path);
    int fd=open(path,O_WRONLY);
    if(fd<0){
        perror("open");
        return 1;
    }
    ssize_t n=write(fd,value,strlen(value));
    if(n<0){
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);
    printf("led %s %s ok\n",led_name,action);
    return 0;
}
