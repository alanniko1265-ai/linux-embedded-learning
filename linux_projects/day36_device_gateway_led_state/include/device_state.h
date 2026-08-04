#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#define DEVICE_NAME_SIZE 64

struct DeviceState{
    char name[DEVICE_NAME_SIZE];
    int online;
    int temperature;
    int voltage;
    int led_on;
};
void device_state_init(struct DeviceState *state);
void device_state_get_status(const struct DeviceState *state,char *buffer,int buffer_size);
void device_state_set_led(struct DeviceState *state,int led_on);
int device_state_get_led(const struct DeviceState *state);
#endif
