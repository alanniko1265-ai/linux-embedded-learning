#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
static int show_winsize(void){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)<0){
        perror("ioctl TIOCGWINSZ");
        return -1;
    }
    printf("rows: %d\n",ws.ws_row);
    printf("cols:%d\n",ws.ws_col);
    return 0;
}
static int show_mmap_file(void){
    int fd;
    struct stat st;
    char *addr;
    fd=open("data/input.txt",O_RDONLY);
    if(fd<0){
        perror("open input");
        return -1;
    }
    if(fstat(fd,&st)<0){
        perror("fstat");
        close(fd);
        return -1;
    }
    if(st.st_size==0){
        printf("file empty\n");
        close(fd);
        return 0;
    }
    addr=mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    if(addr==MAP_FAILED){
        perror("mmap");
        close(fd);
        return -1;
    }
    write(STDOUT_FILENO,addr,st.st_size);
    munmap(addr,st.st_size);
    close(fd);
    return 0;
}
int main(int argc,char *argv[]){
    if(argc<2){ 
        printf("usage: %s <winsize|mmap>\n", argv[0]);
        return 1;
    }
    if(strcmp(argv[1],"winsize")==0){
       if(show_winsize()<0){
        return 1;
       }
    }
    else if(strcmp(argv[1],"mmap")==0){
       if(show_mmap_file()<0){
        return 1;
       }
    }
    else{
        printf("unknown command: %s\n", argv[1]);
        return 1;
    }
    return 0;
}