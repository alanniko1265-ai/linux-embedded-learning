#include<stdio.h>
#include<unistd.h>
int main(void){
    printf("Hello, i.MX6ULL board!\n");
    printf("This program is built for embedded Linux.\n");
    printf("pid=%d\n",getpid());
    return 0;
}