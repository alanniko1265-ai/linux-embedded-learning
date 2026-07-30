#include<stdio.h>
#include<pthread.h>
#define QUEUE_CAPACITY 8
struct DeviceData
{
    int id;
    int value;
};
struct DataQueue{
    struct DeviceData items[QUEUE_CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};
static struct DataQueue g_queue;
static void queue_init(struct DataQueue *queue){
    queue->head=0;
    queue->tail=0;
    queue->count=0;
    pthread_mutex_init(&queue->mutex,NULL);
    pthread_cond_init(&queue->not_empty,NULL);
    pthread_cond_init(&queue->not_full,NULL);
}
static int queue_push(struct DataQueue *queue,struct DeviceData data)
{
    pthread_mutex_lock(&queue->mutex);
    while(queue->count==QUEUE_CAPACITY){
        pthread_cond_wait(&queue->not_full,&queue->mutex);
    }
    queue->items[queue->tail]=data;
    queue->tail=(queue->tail+1)%QUEUE_CAPACITY;
    queue->count++;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}
static int queue_pop(struct DataQueue *queue,struct DeviceData *data){
    pthread_mutex_lock(&queue->mutex);
    while(queue->count==0){
        pthread_cond_wait(&queue->not_empty,&queue->mutex);
    }
    *data=queue->items[queue->head];
    queue->head=(queue->head+1)%QUEUE_CAPACITY;
    queue->count--;
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}
static void *producer_thread(void *arg){
    (void)arg;
    for(int i=0;i<10;i++){
        struct DeviceData data={
            .id=i,
            .value=i*10
        };
        if(queue_push(&g_queue,data)==0){
            printf("[producer] id=%d value=%d\n",data.id,data.value);
        }
        else{
            printf("[producer] queue full,drop id=%d\n",data.id);
        }
    }
    return NULL;
}
static void *consumer_thread(void *arg){
    (void)arg;
    for(int i=0;i<10;i++){
        struct DeviceData data;
        if(queue_pop(&g_queue,&data)==0){
            printf("[consumer] id=%d value=%d\n",data.id,data.value);
        }
    }
    return NULL;
}
int main(void){
    pthread_t producer;
    pthread_t consumer;
    queue_init(&g_queue);
    if(pthread_create(&consumer,NULL,consumer_thread,NULL)!=0){
        perror("pthread_create consumer");
        return 1;
    }
    if(pthread_create(&producer,NULL,producer_thread,NULL)!=0){
        perror("pthread_create producer");
        return 1;
    }
    if(pthread_join(producer,NULL)!=0){
        perror("pthread_join producer");
        return 1;
    }
    if(pthread_join(consumer,NULL)!=0){
        perror("pthread_join consumer");
        return 1;
    }
    return 0;
}