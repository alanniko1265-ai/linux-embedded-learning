#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<limits.h>
#include<sys/stat.h>
int main(int argc,char *argv[]){
    DIR *dir;
    struct dirent *entry;
    if(argc<2){
        printf("usage: %s <dir>\n",argv[0]);
        return 1;
    }
    dir=opendir(argv[1]);
    if(dir==NULL){
        perror("opendir");
        return 1;
    }
    while((entry=readdir(dir))!=NULL){
        if(strcmp(entry->d_name,".")==0 ||strcmp(entry->d_name,"..")==0){
            continue;
        }
        char full_path[PATH_MAX];
        snprintf(full_path,sizeof(full_path),"%s/%s",argv[1],entry->d_name);
        struct stat st;
        if(stat(full_path,&st)<0){
            perror("stat");
            continue;
        }
        if(S_ISREG(st.st_mode)){
            printf("[file] %s %ld bytes\n",full_path,st.st_size);
        }
        else if(S_ISDIR(st.st_mode)){
            printf("[dir ] %s\n",full_path);
        }
        else{
            printf("[other] %s\n",full_path);
        }
    }
    closedir(dir);
    return 0;
}