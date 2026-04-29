#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include "bounded_buffer.h"

void* producer(void* arg)
{
    pthread_setname_np(pthread_self(), "producer");
    bounded_buffer_t* buffer = (bounded_buffer_t*)arg;
    int lValue = rand() % 100;
    while(true)
    {
        printf("Sent to bounded buffer %d\n", lValue);
        bb_enqueue(buffer, (void*)(intptr_t)lValue);
    }

    return NULL;
}

void* consumer(void* arg)
{
    pthread_setname_np(pthread_self(), "consumer");
    bounded_buffer_t* buffer = (bounded_buffer_t*)arg;
    while(true)
    {
        int result = (int)(intptr_t)bb_dequeue(buffer);
        printf("Got from bounded buffer %d\n", result);
    }

    return NULL;
}

int main()
{
    srand(time(NULL));
    bounded_buffer_t* boundedBuffer = bb_create(128);
    const size_t NR_OF_PRODUCERS = 10;
    const size_t NR_OF_CONSUMERS = 10;
    pthread_t* producers = malloc(sizeof(pthread_t) * NR_OF_PRODUCERS);
    pthread_t* consumers = malloc(sizeof(pthread_t) * NR_OF_CONSUMERS);

    for(size_t i=0; i<NR_OF_PRODUCERS; ++i)
    {
        pthread_create(&producers[i], NULL, producer, (void*)boundedBuffer);
    }

    for(size_t i=0; i<NR_OF_CONSUMERS; ++i)
    {
        pthread_create(&consumers[i], NULL, consumer, (void*)boundedBuffer);    
    }
    
    for(size_t i=0; i<NR_OF_CONSUMERS; ++i)
    {
        pthread_join(consumers[i], NULL);
    }
    
    for(size_t i=0; i<NR_OF_PRODUCERS; ++i)
    {
        pthread_join(producers[i], NULL);
    }

    return 0;
}