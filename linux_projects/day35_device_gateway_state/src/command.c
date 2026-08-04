#include<stdio.h>
#include<string.h>
#include "command.h"
#include "device_state.h"
static struct DeviceState g_device_state;
static int g_device_state_inited=0;
void build_response(const char *request,char *response,size_t response_size){
    if(!g_device_state_inited){
        device_state_init(&g_device_state);
        g_device_state_inited=1;
    }
    if(strcmp(request,"status\n")==0){
        device_state_get_status(&g_device_state,response,response_size);
    }
    else if(strcmp(request,"led on\n")==0){
       snprintf(response, response_size, "OK led=on");
    }
    else if(strcmp(request,"led off\n")==0){
        snprintf(response, response_size, "OK led=off");
    }
    else if(strcmp(request,"reboot\n")==0){
        snprintf(response, response_size, "OK reboot scheduled");
    }
    else{
          snprintf(response, response_size, "ERR unknown command");
    }
}