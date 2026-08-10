#include "key_input.h"
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<linux/input.h>

#define KEY_EVENT_PATH "/dev/input/event1"

static void *key_input_thread(void *arg){
    struct DeviceState *state=(struct DeviceState *)arg;
    int fd=open(KEY_EVENT_PATH,O_RDONLY);
    if(fd<0){
        perror("open key event");
        return NULL;
    }
    printf("key input thread started: %s\n",KEY_EVENT_PATH);
    struct input_event event;
    while (1)
    {
        ssize_t n=read(fd,&event,sizeof(event));
        if(n<0){
            perror("read key event");
            break;
        }
        if(n!=sizeof(event)){
            continue;
        }
        if(event.type==EV_KEY && event.code==KEY_0){
            if(event.value==1){
                device_state_set_key(state,1);
                printf("KEY_0 pressed\n");
            }
            else if(event.value==0){
                device_state_set_key(state,0);
                printf("KEY_0 released\n");
            }
        }
    }
    
    close(fd);
    return NULL;
}
int key_input_start(struct DeviceState *state){
    pthread_t thread;
    if(pthread_create(&thread,NULL,key_input_thread,state)!=0){
        perror("pthread_create key input");
        return -1;
    }
    pthread_detach(thread);
    return 0;
}