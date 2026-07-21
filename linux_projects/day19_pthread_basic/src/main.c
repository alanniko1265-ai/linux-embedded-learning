#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
struct ThreadArg
{
    const char *name;
    int count;
};
static int g_count = 0;
static pthread_mutex_t g_count_mutex=PTHREAD_MUTEX_INITIALIZER;
static void *sensor_thread(void *arg){
    struct ThreadArg *thread_arg=(struct ThreadArg *)arg;
    for(int i=0;i<thread_arg->count;i++){
        pthread_mutex_lock(&g_count_mutex);
        g_count++;
        pthread_mutex_unlock(&g_count_mutex);
    }
    return NULL;
}
static void *status_thread(void *arg){
    struct  ThreadArg *thread_arg=(struct ThreadArg*)arg;
     for(int i=0;i<thread_arg->count;i++){
        pthread_mutex_lock(&g_count_mutex);
        g_count++;
        pthread_mutex_unlock(&g_count_mutex);
    }
    return NULL;
    
}
int main(void){
    pthread_t sensor;
    pthread_t status;
    struct ThreadArg sensor_arg =
    {
        .name ="sensor",
        .count=1000000
    };
    struct ThreadArg status_arg={
        .name="status",
        .count=1000000
    };
    printf("main thread start\n");
    if(pthread_create(&sensor,NULL,sensor_thread,&sensor_arg)!=0){
        perror("pthread_create sensor");
        return 1;
    }
    if(pthread_create(&status,NULL,status_thread,&status_arg)!=0){
        perror("pthread_create status");
        return 1;
    }
    if(pthread_join(sensor,NULL)!=0){
        perror("pthread_join sensor");
        return 1;
    }
    if(pthread_join(status,NULL)!=0){
        perror("pthread_join status");
        return 1;
    }
    printf("final count %d\n",g_count);
    printf("main thread exit\n");
    return 0;
}