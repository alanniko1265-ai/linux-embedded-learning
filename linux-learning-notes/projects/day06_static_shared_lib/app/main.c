#include "log.h"

int main(void)
{
    log_info("device gateway started");
    log_warn("serial device is not connected");
    log_error("failed to open config file");

    return 0;
}
