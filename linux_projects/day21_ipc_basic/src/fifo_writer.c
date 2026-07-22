#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#define FIFO_PATH "/tmp/day21_device_fifo"
int main(void){
    const char *message[]={
        "device data: temperature=33\n",
        "device data: humidity=58\n",
        "device data: voltage=24\n"
    };
    int fd=open(FIFO_PATH,O_WRONLY);
    if(fd<0){
        perror("open fifo writer");
        return 1;
    }
    for(int i=0;i<3;i++){
        ssize_t n=write(fd,message[i],strlen(message[i]));
        if(n<0){
            perror("write fifo");
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}