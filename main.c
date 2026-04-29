#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "bounded_buffer.h"

void* producer(void* arg)
{
    pthread_setname_np("producer");
    bounded_buffer_t* buffer = (bounded_buffer_t*)arg;
    int lValue = 5;
    while(true)
    {
        printf("Sent to bounded buffer %d\n", lValue);
        bb_enqueue(buffer, &lValue);
    }

    return NULL;
}

void* consumer(void* arg)
{
    pthread_setname_np("consumer");
    bounded_buffer_t* buffer = (bounded_buffer_t*)arg;
    while(true)
    {
        void* ptr = bb_dequeue(buffer);
        printf("Got from bounded buffer %d\n", *(int*)ptr);
    }

    return NULL;
}

int main()
{
    bounded_buffer_t* boundedBuffer = bb_create(32);
    const size_t NR_OF_PRODUCERS = 4;
    const size_t NR_OF_CONSUMERS = 8;
    pthread_t* producers = malloc(sizeof(pthread_t) * NR_OF_PRODUCERS);
    pthread_t* consumers = malloc(sizeof(pthread_t) * NR_OF_CONSUMERS);

    for(size_t i=0; i<NR_OF_CONSUMERS; ++i)
    {
        pthread_create(&producers[i], NULL, producer, (void*)boundedBuffer);
    }

    for(size_t i=0; i<NR_OF_PRODUCERS; ++i)
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