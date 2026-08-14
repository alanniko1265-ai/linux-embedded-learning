#ifndef RS485_H
#define RS485_H
#include<stddef.h>
#include<sys/types.h>

struct Rs485Device{
    int serial_fd;
    int direction_fd;
};

int rs485_open(struct Rs485Device *device,const char *serial_path,const char *direction_path,int baud_rate);
ssize_t rs485_send(struct Rs485Device *device,const void *buffer,size_t length);
ssize_t rs485_receive(struct Rs485Device *device,void *buffer,size_t buffer_size,int timeout_ms);
void rs485_close(struct Rs485Device *device);

#endif