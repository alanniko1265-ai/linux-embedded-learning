#include "config.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
void config_set_default(struct GatewayConfig *config){
    config->server_port=9000;
    snprintf(config->device_name,sizeof(config->device_name),"demo_gateway");
    snprintf(config->log_path,sizeof(config->log_path),"logs/server.log");
}
int load_config(const char *path,struct GatewayConfig *config){
    config_set_default(config);
    FILE *file=fopen(path,"r");
    if(file==NULL){
        perror("fopen config");
        return -1;
    }
    char line[256];
    while(fgets(line,sizeof(line),file)!=NULL){
       char key[CONFIG_TEXT_SIZE];
       char value[CONFIG_TEXT_SIZE];
       if(sscanf(line,"%127[^=]=%127s",key,value)!=2){
            continue;
       }
       if(strcmp(key,"server_port")==0){
            config->server_port=atoi(value);
       }
       else if(strcmp(key,"device_name")==0){
            snprintf(config->device_name,sizeof(config->device_name),"%s",value);
       }
       else if(strcmp(key,"log_path")==0){
            snprintf(config->log_path,sizeof(config->log_path),"%s",value);
       }
    }
    fclose(file);
    return 0;
}