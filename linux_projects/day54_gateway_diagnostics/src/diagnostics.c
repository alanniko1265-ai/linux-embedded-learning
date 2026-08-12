#include "diagnostics.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define  GATEWAY_VERSION "1.1.0"

static time_t g_start_time;

void diagnostics_init(void){
    g_start_time=time(NULL);
}
void diagnostics_build_response(char *buffer,size_t buffer_size){
    time_t now=time(NULL);
    long uptime_sec=0;
    if(g_start_time>0 && now>=g_start_time){
        uptime_sec=(long)(now-g_start_time);
    }
    snprintf(buffer,buffer_size,"OK code=0 msg=diag version=%s pid=%d uptime_sec=%ld",
    GATEWAY_VERSION,
    getpid(),
    uptime_sec);
}