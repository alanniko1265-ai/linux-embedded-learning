#include "device_state.h"
#include<stdio.h>

void device_state_init(struct DeviceState *state){
    snprintf(state->name,sizeof(state->name),"sensor_node_01");
    state->online=1;
    state->temperature=32;
    state->voltage=24;
}

void  device_state_get_status(const struct DeviceState *state,char *buffer,int buffer_size){
    snprintf(buffer,buffer_size,"OK device=%s online=%d temp=%d voltage=%d",state->name,state->online,state->temperature,state->voltage);
}