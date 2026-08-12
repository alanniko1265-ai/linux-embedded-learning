#include "device_state.h"
#include<stdio.h>
static struct DeviceState g_device_state;
static int g_device_state_inited=0;

struct DeviceState *device_state_global(void){
    if(!g_device_state_inited){
        device_state_init(&g_device_state);
        g_device_state_inited=1;
    }
    return &g_device_state;
}

void device_state_init(struct DeviceState *state){
    snprintf(state->name,sizeof(state->name),"sensor_node_01");
    state->online=1;
    state->temperature=32;
    state->voltage=24;
    state->led_on=0;
    state->key_pressed=0;
}
void  device_state_get_status(const struct DeviceState *state,char *buffer,int buffer_size){
    snprintf(buffer,
            buffer_size,
            "OK code=0 msg=status device=%s online=%d temp=%d voltage=%d led=%s key=%s",
            state->name,
            state->online,
            state->temperature,
            state->voltage,
            state->led_on ? "on" : "off",
            state->key_pressed ? "pressed" : "released"
        );
}
void device_state_set_led(struct DeviceState *state,int led_on){
    state->led_on=led_on ? 1:0;
}
int device_state_get_led(const struct DeviceState *state){
    return state->led_on;
}
void device_state_update_sample(struct DeviceState *state){
    static int sample_count=0;
    state->temperature=32+(sample_count %3);
    state->voltage=24+(sample_count%2);
    sample_count++;
}
void device_state_set_key(struct DeviceState *state,int key_press){
    state->key_pressed=key_press ? 1:0;
}
int device_state_get_key(const struct DeviceState *state){
    return state->key_pressed;
}

