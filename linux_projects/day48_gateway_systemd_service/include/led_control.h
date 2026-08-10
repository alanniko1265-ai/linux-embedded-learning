#ifndef LED_CONTROL_H
#define LED_CONTROL_H

int led_control_set(const char *led_name,int on);
int led_control_get(const char *led_name);

#endif