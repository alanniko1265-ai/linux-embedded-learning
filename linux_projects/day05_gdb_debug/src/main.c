#include <stdio.h>
#include <string.h>

#include "device.h"

static void run_normal_demo(void)
{
    int values[] = {30, 32, 35, 31};
    int count = sizeof(values) / sizeof(values[0]);
    int average = calculate_average(values, count);

    DeviceStatus status = {
        .id = 1001,
        .temperature = average,
        .voltage = 24,
    };

    print_device_status(&status);
}

static void run_logic_bug_demo(void)
{
    int values[] = {30, 32, 35, 31};
    int count = sizeof(values) / sizeof(values[0]);
    int average = calculate_average_buggy(values, count);

    printf("buggy average: %d\n", average);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s normal|logic-bug|crash\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "normal") == 0) {
        run_normal_demo();
        return 0;
    }

    if (strcmp(argv[1], "logic-bug") == 0) {
        run_logic_bug_demo();
        return 0;
    }

    if (strcmp(argv[1], "crash") == 0) {
        crash_by_null_pointer();
        return 0;
    }

    printf("unknown mode: %s\n", argv[1]);
    return 1;
}
