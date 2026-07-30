#include "logger.h"
#include <stdio.h>
#include <time.h>
static FILE *g_log_file =NULL;
int logger_init(const char *path){
    g_log_file=fopen(path,"a");
    if(g_log_file==NULL){
        perror("fopen");
        return -1;
    }
    return 0;
}
static void write_log(const char *level, const char *message)
{
    time_t now;
    struct tm *tm_info;
    char time_text[32];

    if (g_log_file == NULL) {
        return;
    }

    now = time(NULL);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        return;
    }

    strftime(time_text, sizeof(time_text), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(g_log_file, "[%s] [%s] %s\n", time_text, level, message);
    fflush(g_log_file);
}
void logger_info(const char *message){
    write_log("INFO",message);
}
void logger_warn(const char *message){
    write_log("WARN",message);
}
void logger_error(const char *message){
    write_log("ERROR",message);
}
void logger_close(void){
    if(g_log_file!=NULL){
        fclose(g_log_file);
        g_log_file=NULL;
    }
}