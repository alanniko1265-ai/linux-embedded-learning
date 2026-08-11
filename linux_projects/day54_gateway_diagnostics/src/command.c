#include<stdio.h>
#include<string.h>
#include "command.h"
#include "device_state.h"
#include "led_control.h"
#include "diagnostics.h"
void build_response(const char *request,char *response,size_t response_size){
    struct DeviceState *state=device_state_global();
    if(strcmp(request,"status\n")==0){
        int led_on=led_control_get("red");
        if(led_on>=0){
            device_state_set_led(state,led_on);    
        }
        device_state_update_sample(state);
        device_state_get_status(state,response,response_size);
    }
    else if(strcmp(request,"led on\n")==0){
        if(led_control_set("red",1)<0){
            snprintf(response,response_size,"ERR code=2001 msg=led_on_failed");
            return;
        }
        device_state_set_led(state,1);
        snprintf(response, response_size, "OK code=%d msg=led_on",RESPONSE_OK);
    }
    else if(strcmp(request,"led off\n")==0){
        if(led_control_set("red",0)<0){
            snprintf(response,response_size,"ERR code=2002 msg=led_off_failed");
            return;
        }
        device_state_set_led(state,0);
        snprintf(response, response_size, "OK code=%d msg=led_off",RESPONSE_OK);
    }
    else if(strcmp(request,"reboot\n")==0){
        snprintf(response, response_size, "OK code=%d msg=reboot_scheduled",RESPONSE_OK);
    }
    else if(strcmp(request,"diag\n")==0){
        diagnostics_build_response(response,response_size);
    }
    else{
          snprintf(response, response_size, "ERR code=%d msg=unknown_command",RESPONSE_ERR_UNKNOWN_COMMAND);
    }
}