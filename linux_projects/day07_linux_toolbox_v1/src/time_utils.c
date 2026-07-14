#include<stdio.h>
#include<time.h>
#include "time_utils.h"
void print_current_time(void){
    time_t now=time(NULL);
    struct tm *local=localtime(&now);
    if(local==NULL){
        printf("failed to get local time\n");
        return;
    }
    printf("%04d-%02d-%02d %02d:%02d:%02d\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);
}