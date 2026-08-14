#define _DEFAULT_SOURCE
#include "rs485.h"
#include<errno.h>
#include<fcntl.h>
#include<poll.h>
#include<unistd.h>
#include<termios.h>

static speed_t baud_to_speed(int baud_rate){
    switch (baud_rate)
    {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 115200:
        return B115200;
    default:
        errno=EINVAL;
        return (speed_t)-1;
    }
}
static int set_direction(struct Rs485Device *device,int transmit){
    char value;
    ssize_t written;
    if(device == NULL||device->direction_fd<0){
        errno=EINVAL;
        return -1;
    }
    value=transmit ? '1' : '0';
    if(lseek(device->direction_fd,0,SEEK_SET)<0){
        return -1;
    }
    do{
        written=write(device->direction_fd,&value,1);
    }while(written<0 && errno==EINTR);
    if(written!=1){
        if(written>=0){
            errno=EIO;
        }
        return -1;
    }
    return 0;
}

ssize_t rs485_send(struct Rs485Device *device,const void *buffer,size_t length){
    const unsigned char *bytes=buffer;
    size_t total=0;
    ssize_t written;
    int saved_errno;
    if(device==NULL||device->serial_fd<0||(buffer==NULL&&length>0)){
        errno=EINVAL;
        return -1;
    }
    if(length==0){
        return 0;
    }
    if(set_direction(device,1)<0){
        return -1;
    }
    while(total<length){
        written=write(device->serial_fd,bytes+total,length-total);
        if(written<0){
            if(errno==EINTR){
                continue;
            }
            saved_errno=errno;
            set_direction(device,0);
            errno=saved_errno;
            return -1;
        }
        if(written==0){
            saved_errno=EIO;
            set_direction(device,0);
            errno=saved_errno;
            return -1;
        }
        total+=(size_t)written;
    }
    if(tcdrain(device->serial_fd)<0){
        saved_errno=errno;
        set_direction(device,0);
        errno=saved_errno;
        return -1;
    }
    if(set_direction(device,0)<0){
        return -1;
    }
    return (ssize_t)total;
}
ssize_t rs485_receive(struct Rs485Device *device,void *buffer,size_t buffer_size,int timeout_ms){
    struct pollfd event;
    int ready;
    if(device==NULL||device->serial_fd<0||buffer==NULL||buffer_size==0||timeout_ms<0){
        errno=EINVAL;
        return -1;
    }
    if(set_direction(device,0)<0){
        return -1;
    }
    event.fd=device->serial_fd;
    event.events=POLLIN;
    event.revents=0;
    do{
        ready=poll(&event,1,timeout_ms);
    }while(ready<0 && errno ==EINTR);
    if(ready<0){
        return -1;
    }
    if(ready==0){
        return 0;
    }
    if(event.revents & (POLLERR|POLLHUP|POLLNVAL)){
        errno=EIO;
        return -1;
    }
   if(!(event.revents&POLLIN)){
    errno=EIO;
    return -1;
   }
    ssize_t received;
    do{
        received=read(device->serial_fd,buffer,buffer_size);
    }while ((received<0&&errno==EINTR));
    return received;
}
int rs485_open(struct Rs485Device *device,const char *serial_path,const char *direction_path,int baud_rate){
    if(device==NULL||serial_path==NULL||direction_path==NULL){
        errno=EINVAL;
        return -1;
    }
    device->serial_fd=-1;
    device->direction_fd=-1;
    device->serial_fd=open(serial_path,O_RDWR|O_NOCTTY);
    if(device->serial_fd<0){
        return -1;
    }
    device->direction_fd=open(direction_path,O_WRONLY);
    if(device->direction_fd<0){
        close(device->serial_fd);
        device->serial_fd=-1;
        return -1;
    }
    struct termios options;
    speed_t speed=baud_to_speed(baud_rate);
    if(speed == (speed_t)-1){
        goto fail;
    }
    if(tcgetattr(device->serial_fd,&options)<0){
        goto fail;
    }
    cfmakeraw(&options);
    options.c_cflag &= ~(CSIZE|PARENB|CSTOPB|CRTSCTS);
    options.c_cflag |= CS8|CLOCAL|CREAD;
    options.c_cc[VMIN]=1;
    options.c_cc[VTIME]=0;
    if(cfsetispeed(&options,speed)<0||cfsetospeed(&options,speed)<0){
        goto fail;
    }
    if(tcsetattr(device->serial_fd,TCSANOW,&options)<0){
        goto fail;
    }
    if(set_direction(device,0)<0){
        goto fail;
    }
    return 0;
    fail:{
        int saved_errno=errno;
        close(device->direction_fd);
        close(device->serial_fd);
        device->direction_fd=-1;
        device->serial_fd=-1;
        errno=saved_errno;
        return -1;
    }
}
void rs485_close(struct Rs485Device *device){
    if(device==NULL){
        return;
    }
    if(device->direction_fd>=0){
        set_direction(device,0);
    }
    if(device->serial_fd>=0){
        close(device->serial_fd);
        device->serial_fd=-1;
    }
    if(device->direction_fd>=0){
        close(device->direction_fd);
        device->direction_fd=-1;
    }
}
