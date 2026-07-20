#include<stdio.h>
#include "logger.h"
int main(){
    if(logger_init("logs/app.log")<0){
        return 1;
    }
    printf("logger init ok\n");
    logger_info("logger module started");
    logger_info("device service init simulated");
    logger_warn("device temperature is close to limit");
    logger_error("sensor read failed simulated");
    logger_close();
    printf("logger closed\n");




    return 0;
}