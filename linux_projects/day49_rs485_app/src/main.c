#include "rs485.h"
#include<stdio.h>
#include<string.h>

#define SERIAL_PATH "/dev/ttymxc1"
#define DIRECTION_PATH "/sys/class/gpio/gpio22/value"
#define BAUD_RATE 9600
#define SEND_BUFFER_SIZE 256
#define RECEIVE_BUFFER_SIZE 256
#define RECEIVE_TIMEOUT_MS 15000

static void print_usage(const char *program){
    printf("Usage:\n");
    printf(" %s send <message>\n",program);
    printf(" %s receive\n",program);
}
int main(int argc,char *argv[]){
    struct Rs485Device device;
    char send_buffer[SEND_BUFFER_SIZE];
    char receive_buffer[RECEIVE_BUFFER_SIZE+1];
    int message_length;
    ssize_t result;
    int send_mode;
    if(argc<2){
        print_usage(argv[0]);
        return 1;
    }
    if(strcmp(argv[1],"send")==0){
        if(argc!=3){
            print_usage(argv[0]);
            return 1;
        }
        send_mode=1;
    }
    else if(strcmp(argv[1],"receive")==0){
        if(argc!=2){
            print_usage(argv[0]);
            return 1;
        }
        send_mode=0;
    }
    else{
        printf("unknown cpmmand: %s\n",argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    if(rs485_open(&device,SERIAL_PATH,DIRECTION_PATH,BAUD_RATE)<0){
        perror("rs485_open");
        return 1;
    }
    if(send_mode){
        message_length=snprintf(send_buffer,sizeof(send_buffer),"%s\n",argv[2]);
        if(message_length<0||(size_t)message_length>=sizeof(send_buffer)){
            fprintf(stderr,"message is too long\n");
            rs485_close(&device);
            return 1;
        }
        result=rs485_send(&device,send_buffer,(size_t)message_length);
        if(result<0){
            perror("rs485_send");
            rs485_close(&device);
            return 1;
        }
        printf("sent %zd bytes\n",result);
    }
    else{
        printf("waiting for RS485 date...\n");
        result=rs485_receive(&device,receive_buffer,RECEIVE_BUFFER_SIZE,RECEIVE_TIMEOUT_MS);
        if(result<0){
            perror("rs485_receive");
            rs485_close(&device);
            return 1;
        }
        if(result==0){
            printf("receive timeout\n");
        }
        else{
            receive_buffer[result]='\0';
            printf("received %zd bytes: %s",result,receive_buffer);
            if(receive_buffer[result-1]!='\n'){
                putchar('\n');
            }
        }
    }
    rs485_close(&device);
    return 0;
}