#include "bounded_buffer.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

struct bounded_buffer {
    void **items;              // Circular buffer array
    size_t capacity;           // Max items
    size_t head;               // Next write position
    size_t tail;               // Next read position
    size_t count;              // Current number of items
    
    pthread_mutex_t lock;      // Protects all shared state
    pthread_cond_t not_full;   // Producers wait here
    pthread_cond_t not_empty;  // Consumers wait here
    
    bool closed;               // Set by bb_close()
};

bounded_buffer_t* bb_create(size_t capacity)
{
    bounded_buffer_t* lBoundedBuffer = malloc(sizeof(bounded_buffer_t));
    lBoundedBuffer->capacity = capacity;
    lBoundedBuffer->head = 0;
    lBoundedBuffer->tail = 0;
    lBoundedBuffer->count = 0;
    lBoundedBuffer->closed = false;

    lBoundedBuffer->items = malloc(sizeof(void*) * capacity);

    pthread_mutex_init(&lBoundedBuffer->lock, NULL);
    pthread_cond_init(&lBoundedBuffer->not_empty, NULL);
    pthread_cond_init(&lBoundedBuffer->not_full, NULL);

    return lBoundedBuffer;
}

int bb_enqueue(bounded_buffer_t *bb, void *item) {
    pthread_mutex_lock(&bb->lock);
    
    // Wait while buffer is full AND not closed
    while (bb->count == bb->capacity && !bb->closed) {
        pthread_cond_wait(&bb->not_full, &bb->lock);
        // Mutex is atomically released while waiting,
        // then re-acquired before returning
    }
    
    // Check if we woke up because buffer closed
    if (bb->closed) {
        pthread_mutex_unlock(&bb->lock);
        return -1;
    }
    
    // Insert item into circular buffer
    bb->items[bb->head] = item;
    bb->head = (bb->head + 1) % bb->capacity;  // Wrap around
    bb->count++;
    
    // Wake up one waiting consumer
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->lock);

    return 0;
}

void* bb_dequeue(bounded_buffer_t *bb)
{
    pthread_mutex_lock(&bb->lock);

    while (bb->count == 0 && !bb->closed) {
        pthread_cond_wait(&bb->not_empty, &bb->lock);
    }

    // Drain remaining items even after close; only fail when truly empty.
    if (bb->count == 0 && bb->closed) {
        pthread_mutex_unlock(&bb->lock);
        return NULL;
    }

    void *item = bb->items[bb->tail];
    bb->items[bb->tail] = NULL;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count--;

    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->lock);
    return item;
}