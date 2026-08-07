#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/input.h>
#include<string.h>
#include<limits.h>

#define KEY_EVENT_PATH "/dev/input/event1"
#define LED_BRIGHTNESS_PATH "/sys/class/leds/green/brightness"
static int set_red_led(int on){
    const char *value= on ? "1" : "0";
    int fd=open(LED_BRIGHTNESS_PATH,O_WRONLY);
    if(fd<0){
        perror("open green led");
        return -1;
    }
    ssize_t n=write(fd,value,strlen(value));
    if(n<0){
        perror("write green led");
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}
int main(void){
    int fd=open(KEY_EVENT_PATH,O_RDONLY);
    if(fd<0){
        perror("open key event");
        return 1;
    }
    printf("listening key events from %s\n",KEY_EVENT_PATH);
    printf("press Ctrl+C to exit\n");
    struct input_event event;
    int led_state=0;
    while (1)
    {
        ssize_t n=read(fd,&event,sizeof(event));
        if(n<0){
            perror("read key event");
            close(fd);
            return 1;
        }
        if(n!=sizeof(event)){
            printf("read incomplete input event\n");
            continue;
        }
        if(event.type==EV_KEY && event.code==KEY_0){
            if(event.value==1){
                led_state=!led_state;
                if(set_red_led(led_state)<0){
                    printf("set red led failed\n");
                }
                printf("KEY_0 pressed, led_state=%s\n", led_state ? "on" : "off");
            }
            else if(event.value==0){
                printf("KEY_0 released\n");
            }
            else if(event.value==2){
                printf("KEY_0 repeated\n");
            }
        }
    }
    
    close(fd);
}
