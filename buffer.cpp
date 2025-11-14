#include "buffer.h"
#include <cstdio>

//Variables

sem_t full;
sem_t empty;
pthread_mutex_t mutex;
int in = 0;
int out = 0;
int count = 0;
buffer_item shared_buffer[BUFFER_SIZE];
pthread_mutex_t counter_mutex;
int times_buffer_empty = 0;
int times_buffer_full = 0;

//Initialize Buffer

void buffer_init(){
    
    for(int i = 0; i < BUFFER_SIZE; i++){ //initialize to -1
        shared_buffer[i] = -1;
    }
    sem_init(&full, 0, 0); //initialize full semepahore
    sem_init(&empty, 0, BUFFER_SIZE); //initialize empty semaphore
    pthread_mutex_init(&mutex, nullptr); //initialize main mutex
    pthread_mutex_init(&counter_mutex, nullptr); //initialize counter mutex

}

//Insert in Buffer

bool buffer_insert_item( buffer_item item ){  
    
    int val;
    sem_getvalue(&empty, &val);

    if (val == 0) {

         printf("All buffers full. Producer %lu waits.\n", pthread_self());
         pthread_mutex_lock(&counter_mutex);
         times_buffer_full++;
         pthread_mutex_unlock(&counter_mutex);
        
    }

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    shared_buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    return true;
}

//Read Item From Buffer

bool buffer_remove_item( buffer_item *item ){
    
    int val;
    sem_getvalue(&full, &val);
    
    if (val == 0) {

         printf("All buffers empty. Consumer %lu waits.\n", pthread_self());
         pthread_mutex_lock(&counter_mutex);
         times_buffer_empty++;
         pthread_mutex_unlock(&counter_mutex);

    }

    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    *item = shared_buffer[out]; //not neccesary
    out = (out + 1) % BUFFER_SIZE;
    count--;

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

    return true;
};