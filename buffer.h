#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

#include <semaphore.h>
#include <pthread.h>

typedef int buffer_item;

#define BUFFER_SIZE 5

//External Global Variables

extern sem_t full;
extern sem_t empty;
extern pthread_mutex_t mutex;
extern pthread_mutex_t waiting_mutex;
extern int in;
extern int out;
extern int count;
extern buffer_item shared_buffer[BUFFER_SIZE];
extern int times_buffer_empty;
extern int times_buffer_full;

//Argument struct for the producer and consumer threads

struct thread_arg {
    int sleep_time;
    int index;
};

//********************************************************************
//
// Buffer Initialization Function
//
// Sets buffer to -1 and initializes the semaphores and mutex locks
//
// Return Value
// ------------
// void                       
//
// Value Parameters
// ----------------
// 
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
void buffer_init();

//********************************************************************
//
// Buffer Insert Function
//
// Inserts an item to the buffer
//
// Return Value
// ------------
// bool                 True/False if item inserted properly                       
//
// Value Parameters
// ----------------
// item                 buffer_item         the item to be inserted
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
bool buffer_insert_item(buffer_item item);

//********************************************************************
//
// Buffer Read Function
//
// Reads an item from the buffer
//
// Return Value
// ------------
// bool                 True/False if item was read properly                       
//
// Value Parameters
// ----------------
// 
//
// Reference Parameters
// --------------------
// item                 buffer_item         the item to be read
//
//*******************************************************************
bool buffer_remove_item(buffer_item *item);

#endif // _BUFFER_H_DEFINED_
