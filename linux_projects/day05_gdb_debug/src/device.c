#include <stdio.h>

#include "device.h"

void print_device_status(const DeviceStatus *status)
{
    printf("device id: %d\n", status->id);
    printf("temperature: %d\n", status->temperature);
    printf("voltage: %d\n", status->voltage);
}

int calculate_average(const int *values, int count)
{
    int sum = 0;

    for (int i = 0; i < count; i++) {
        sum += values[i];
    }

    return sum / count;
}

int calculate_average_buggy(const int *values, int count)
{
    int sum = 0;

    for (int i = 0; i <= count; i++) {
        sum += values[i];
    }

    return sum / count;
}

void crash_by_null_pointer(void)
{
    DeviceStatus *status = NULL;

    print_device_status(status);
}
