#include<stdio.h>
#include<string.h>
#include "command.h"
#include "device_state.h"
#include "led_control.h"
static struct DeviceState g_device_state;
static int g_device_state_inited=0;
void build_response(const char *request,char *response,size_t response_size){
    if(!g_device_state_inited){
        device_state_init(&g_device_state);
        g_device_state_inited=1;
    }
    if(strcmp(request,"status\n")==0){
        int led_on=led_control_get("red");
        if(led_on>=0){
            device_state_set_led(&g_device_state,led_on);    
        }
        device_state_update_sample(&g_device_state);
        device_state_get_status(&g_device_state,response,response_size);
    }
    else if(strcmp(request,"led on\n")==0){
        if(led_control_set("red",1)<0){
            snprintf(response,response_size,"ERR code=2001 msg=led_on_failed");
            return;
        }
        device_state_set_led(&g_device_state,1);
        snprintf(response, response_size, "OK code=%d msg=led_on",RESPONSE_OK);
    }
    else if(strcmp(request,"led off\n")==0){
        if(led_control_set("red",0)<0){
            snprintf(response,response_size,"ERR code=2002 msg=led_off_failed");
            return;
        }
        device_state_set_led(&g_device_state,0);
        snprintf(response, response_size, "OK code=%d msg=led_off",RESPONSE_OK);
    }
    else if(strcmp(request,"reboot\n")==0){
        snprintf(response, response_size, "OK code=%d msg=reboot_scheduled",RESPONSE_OK);
    }
    else{
          snprintf(response, response_size, "ERR code=%d msg=unknown_command",RESPONSE_ERR_UNKNOWN_COMMAND);
    }
}