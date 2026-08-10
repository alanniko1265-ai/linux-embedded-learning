#include "led_control.h"

#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<limits.h>

#define LED_BASE_PATH "/sys/class/leds"

static int is_supported_led(const char *led_name){
    if(strcmp(led_name,"red")==0||strcmp(led_name,"blue")==0||strcmp(led_name,"green")==0){
        return 1;
    }
    return 0;
}
static int build_brightness_path(const char *led_name,char *path,size_t path_size){
    if(!is_supported_led(led_name)){
        return -1;
    }
    snprintf(path,path_size,"%s/%s/brightness",LED_BASE_PATH,led_name);
    return 0;
}
int led_control_set(const char *led_name,int on){
    char path[256];
    if(build_brightness_path(led_name,path,sizeof(path))<0){
        return -1;
    }
    const char *value = on ? "1" : "0";
    int fd=open(path,O_WRONLY);
    if(fd<0){
        perror("open led brightness");
        return -1;
    }
    ssize_t n=write(fd,value,strlen(value));
    if(n<0){
        perror("write led brightness");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}
int led_control_get(const char *led_name){
    char path[256];
    if(build_brightness_path(led_name,path,sizeof(path))<0){
        return -1;
    }
    int fd=open(path,O_RDONLY);
    if(fd<0){
        perror("open led brightness");
        return -1;
    }
    char value='0';
    ssize_t n=read(fd,&value,1);
    if(n<0){
        perror("read led brightness");
        close(fd);
        return -1;
    }
    close(fd);
    return value== '0' ? 0:1;
}