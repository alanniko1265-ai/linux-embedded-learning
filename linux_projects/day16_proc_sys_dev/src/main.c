#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
static int show_cpuinfo(void){
    FILE *fp;
    char line[256];
    int count=0;
    fp=fopen("/proc/cpuinfo","r");
    if(fp==NULL){
        perror("fopen cpuinfo");
        return -1;
    }
    while(fgets(line,sizeof(line),fp)!=NULL&& count<10){
        printf("%s",line);
        count++;
    }
    fclose(fp);
    return 0;
}
static int show_meminfo(void){
    int count=0;
    char line[256];
    FILE *fp;
    fp=fopen("/proc/meminfo","r");
    if(fp==NULL){
        perror("fopen meminfo");
        return -1;
    }
    while(fgets(line,sizeof(line),fp)!=NULL && count<10){
        printf("%s",line);
        count++;
    }
    fclose(fp);
    return 0;
}
static int test_dev_null(void){
    int fd;
    const char *message="hello /dev/null\n";
    ssize_t n;
    fd=open("/dev/null",O_WRONLY);
    if(fd<0){
        perror("open /dev/null");
        return -1;
    }
    n=write(fd,message,strlen(message));
    if(n<0){
        perror("write /dev/null");
        close(fd);
        return -1;
    }
    printf("write %ld bytes to /dev/null\n",n);
    close(fd);
    return 0;
}
static int test_dev_zero(void){
    int fd;
    unsigned char buffer[16];
    ssize_t n;
    fd=open("/dev/zero",O_RDONLY);
    if(fd<0){
        perror("open /dev/zero");
        return -1;
    }
    n=read(fd,buffer,sizeof(buffer));
    if(n<0){
        perror("read /dev/zero");
        close(fd);
        return -1;
    }
    printf("read %ld bytes from /dev/zero\n",n);
    for(ssize_t i=0;i<n;i++){
        printf("%02x ",buffer[i]);
    }
    printf("\n");
    close(fd);
    return 0;
}
int main(int argc,char *argv[]){
    if(argc<2){
        printf("usage: %s <cpu|mem|null|zero>\n",argv[0]);
        return 1;
    }
    if(strcmp(argv[1],"cpu")==0){
        if(show_cpuinfo()<0){
            return 1;
        }
    }
    else if(strcmp(argv[1],"mem")==0){
        if(show_meminfo()<0){
            return 1;
        }
 
    }
    else if(strcmp(argv[1],"null")==0){
        if(test_dev_null()<0){
            return 1;
        }
    }
    else if(strcmp(argv[1],"zero")==0){
        if(test_dev_zero()<0){
            return 1;
        }
    }
    else{
        printf("unknown command: %s\n",argv[1]);
        return 1;
    }
    return 0;
}