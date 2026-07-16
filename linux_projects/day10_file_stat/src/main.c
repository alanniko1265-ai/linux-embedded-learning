#include<stdio.h>
#include<sys/stat.h>
int main(int argc,char *argv[]){
    struct stat st;
    if(argc<2){
        printf("usage %s <path>\n",argv[0]);
        return 1;
    }
    if(stat(argv[1],&st)<0){
        perror("stat");
        return 1;
    }
    printf("path %s\n",argv[1]);
    printf("size: %ld bytes\n",st.st_size);
    if(S_ISREG(st.st_mode)){
        printf("type: regular file\n");
    }
    else if(S_ISDIR(st.st_mode)){
        printf("type: directory\n");
    }
    else{
        printf("type: other\n");
    }
    char type_char;
    if(S_ISREG(st.st_mode)){
        type_char='-';
    }
    else if(S_ISDIR(st.st_mode)){
        type_char='d';
    }
    else{
        type_char='?';
    }
    printf("mode: %c%c%c%c%c%c%c%c%c%c\n",type_char,(st.st_mode & S_IRUSR) ? 'r' : '-',(st.st_mode & S_IWUSR) ? 'w': '-',(st.st_mode &S_IXUSR) ? 'x': '-',(st.st_mode & S_IRGRP) ? 'r' : '-',(st.st_mode & S_IWGRP) ? 'w': '-',(st.st_mode &S_IXGRP) ? 'x': '-',(st.st_mode & S_IROTH) ? 'r' : '-',(st.st_mode & S_IWOTH) ? 'w': '-',(st.st_mode &S_IXOTH) ? 'x': '-');

    return 0;
}