#include <stdio.h>
#include <time.h>
#include "log.h"
void print_log(const char *message){
   time_t now=time(NULL);
   struct tm *local=localtime(&now);
   if(local==NULL){
      printf("[INFO] %s\n",message);
      return;
}   
   printf("[%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec,message);
}   