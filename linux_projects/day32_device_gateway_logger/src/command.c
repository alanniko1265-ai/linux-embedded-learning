#include<stdio.h>
#include<string.h>
#include "command.h"
void build_response(const char *request,char *response,size_t response_size){
    if(strcmp(request,"status\n")==0){
        snprintf(response,response_size, "OK device=online temp=32 voltage=24");
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