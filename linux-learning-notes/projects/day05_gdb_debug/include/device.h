#ifndef DEVICE_H
#define DEVICE_H

typedef struct {
    int id;
    int temperature;
    int voltage;
} DeviceStatus;

void print_device_status(const DeviceStatus *status);
int calculate_average(const int *values, int count);
int calculate_average_buggy(const int *values, int count);
void crash_by_null_pointer(void);

#endif
