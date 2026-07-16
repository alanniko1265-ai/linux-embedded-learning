#include <stdio.h>
#include <time.h>

#include "log.h"

static void print_log(const char *level, const char *message)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (local == NULL) {
        printf("[%s] %s\n", level, message);
        return;
    }

    printf("[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
           local->tm_year + 1900,
           local->tm_mon + 1,
           local->tm_mday,
           local->tm_hour,
           local->tm_min,
           local->tm_sec,
           level,
           message);
}

void log_info(const char *message)
{
    print_log("INFO", message);
}

void log_warn(const char *message)
{
    print_log("WARN", message);
}

void log_error(const char *message)
{
    print_log("ERROR", message);
}
