//********************************************************************
//
// Drew Dean
// October 8th 2025
//
//********************************************************************

#include <stdlib.h> 
#include "buffer.h"
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <pthread.h>
#include <cmath>
#include <vector>
#include <string>

//Namespace

using namespace std;

//Global Variables

bool running = true;
string display;
vector<int> producer_total;
vector<int> consumer_total;

//Pthread Locks

pthread_mutex_t print_mutex;
pthread_mutex_t consumer_lock;

//Function Prototypes


//********************************************************************
//
// Print Buffer Status Function
//
// This function prints out the buffers contents along with the locations
// of the R and W variables
//
// Return Value
// ------------
// void                       
//
// Value Parameters
// ----------------
// type            int            0 for producer 1 for consumer
// item            buffer_item    the item to be inserted or read
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
void print_status(int type, buffer_item item);

//********************************************************************
//
// Producer Function
//
// This function repeatedly sleeps for a random period of time 
// and then inserts an item into the buffer
//
// Return Value
// ------------
// void                       
//
// Value Parameters
// ----------------
// *param           void         a thread_arg struct for the arguments
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
void *producer(void *param);

//********************************************************************
//
// Prime Function
//
// This function checks whether the number is a prime number or not
//
// Return Value
// ------------
// bool                             True/False if the number is prime    
//
// Value Parameters
// ----------------
// n                int             The number to be checked for prime status
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
bool isPrime(int n);

//********************************************************************
//
// Consumer Function
//
// This function repeatedly sleeps for a random period of time 
// and then reads an item from the buffer
//
// Return Value
// ------------
// void                       
//
// Value Parameters
// ----------------
// *param           void        a thread_arg struct for the arguments
//
// Reference Parameters
// --------------------
// 
//
//*******************************************************************
void *consumer(void *param);

/********************************************************************************/

int main( int argc, char *argv[] )
{

srand(time(NULL)); //random seed

//Get Command Arguments

int sleep_time = atoi(argv[1]);
int thread_sleep_time = atoi(argv[2]);
int producer_count = atoi(argv[3]);
int consumer_count = atoi(argv[4]);

//Variables

display = argv[5];
int num_threads = 0;

thread_arg P_args[producer_count];
thread_arg C_args[consumer_count];

vector<pthread_t> producer_tids(producer_count);
vector<pthread_t> consumer_tids(consumer_count);
pthread_attr_t attr;

//Prepare Vectors

producer_total.resize(producer_count, 0);
consumer_total.resize(consumer_count, 0);

//Initialization

pthread_mutex_init(&print_mutex, nullptr);
pthread_mutex_init(&consumer_lock, nullptr);
pthread_attr_init(&attr);

buffer_init();

//Create Threads

cout << "Starting Threads" << endl;

for(int i = 0; i < producer_count; i++){
    P_args[i].sleep_time = thread_sleep_time;
    P_args[i].index = i;
    pthread_create(&producer_tids[i], &attr, producer, &P_args[i]);
}

for(int i = 0; i < consumer_count; i++){
    C_args[i].sleep_time = thread_sleep_time;
    C_args[i].index = i;
    pthread_create(&consumer_tids[i], &attr, consumer, &C_args[i]);
}

//Ending Threads

sleep(sleep_time);

running = false; //Triggers Threads to end

for(int i = 0; i < producer_count; i++){
    pthread_join(producer_tids[i], NULL);
}

for(int i = 0; i < consumer_count; i++){
    pthread_join(consumer_tids[i], NULL);
}

//Calculate Totals

int total_produced = 0;
for (int i = 0; i < producer_total.size(); i++){
    total_produced += producer_total[i];
}
int total_consumed = 0;
for (int i = 0; i < consumer_total.size(); i++){
    total_consumed += consumer_total[i];
}

//Print Final Statistics

printf("PRODUCER / CONSUMER SIMULATION COMPLETE\n");
printf("=======================================\n");
printf("Simulation Time:                       %d\n", sleep_time);
printf("Maximum Thread Sleep Time:             %d\n", thread_sleep_time);
printf("Number of Producer Threads:            %d\n", producer_count);
printf("Number of Consumer Threads:            %d\n", consumer_count);
printf("Size of Buffer:                        %d\n", BUFFER_SIZE);
printf("\n");
printf("Total Number of Items Produced:        %d\n", total_produced);
for(int i = 0; i < producer_count; i++){
    printf("   Thread %d:                      %d\n", i, producer_total[i]);
}
printf("Total Number of Items Consumed:        %d\n", total_consumed);
for(int i = 0; i < consumer_count; i++){
    printf("   Thread %d:                      %d\n", (i + producer_count), consumer_total[i]);
}
printf("Number of Items Remaining in Buffer:   %d\n", count);
printf("Number of Times Buffer was Full:       %d\n", times_buffer_full);
printf("Number of Times Buffer was Empty:      %d\n", times_buffer_empty);

}

/********************************************************************************/

bool isPrime(int n) {
    
    if (n <= 1) {
        return false;
    }

    
    for (int i = 2; i <= sqrt(n); ++i) {
        if (n % i == 0) {
            
            return false;
        }
    }

    
    return true;
}

void *producer(void *param)
{

//Variables

thread_arg *p = (thread_arg*)param; //arguments
int sleep_time = p->sleep_time;
int index = p->index;
buffer_item item; //item to insert

//Produce

while(running)
{

    sleep(rand() % sleep_time + 1); //Random Sleep Time

    item = (rand() % 100) + 1; //Random Item Below 100

    if(!buffer_insert_item(item)){
        
        printf("ERROR");

    }
    else{
        
        producer_total[index]++; //Increment This Buffer's Total
        print_status(0, item); //Print Buffer Status

    }

}

return NULL;

}

void *consumer(void *param)
{

//Variables

thread_arg *p = (thread_arg*)param; //arguments
int sleep_time = p->sleep_time;
int index = p->index;
buffer_item item; //item to insert

//Consume

while(1)
{

    sleep(rand() % sleep_time + 1); //Random Sleep Time

    pthread_mutex_lock(&consumer_lock);

    bool done = !running && count == 0; //Check if done running AND buffer empty

    pthread_mutex_unlock(&consumer_lock);

    if(done){
        break; //if empty and done running then end the threads
    }

    if(!buffer_remove_item(&item)){
        
        printf("ERROR2");

    }
    else{

        consumer_total[index]++; //increment consumer total
        print_status(1, item); // print the buffer status

    }

}

return NULL;
}

void print_status(int type, buffer_item item){
    
    pthread_mutex_lock(&print_mutex); //lock printing so prints don't overlap

    if(display == "yes"){

        if ((type == 0)){
            printf("producer %lu writes %d\n", pthread_self(), item); //producer line
        }
        else if(type == 1){
            printf("consumer %lu read %d", pthread_self(), item); //Consumer line

            if(isPrime(item)){
                printf(" ***PRIME***\n"); //Adds Prime tag if its prime
            }
            else{
                printf("\n");
            }
        }

        //Prints the buffer and where read && write is

        cout << "(buffers occupied: " << count << ")" << endl;
        printf("buffers: %d %d %d %d %d\n", shared_buffer[0], shared_buffer[1], 
            shared_buffer[2], shared_buffer[3], shared_buffer[4]);

        printf("---- ---- ---- ---- ----\n");

        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (i == out && i == in)
                printf(" WR  ");
            else if (i == out)
                printf("  R  ");
            else if (i == in)
                printf("  W  ");
            else
                printf("     ");
        }

        printf("\n");

        
    }


    pthread_mutex_unlock(&print_mutex);

}
