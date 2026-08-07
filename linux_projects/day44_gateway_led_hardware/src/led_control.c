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
