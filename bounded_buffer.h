// bounded_buffer.h

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <pthread.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct bounded_buffer bounded_buffer_t;

// Create a bounded buffer with fixed capacity
// Returns NULL on failure
bounded_buffer_t* bb_create(size_t capacity);

// Producer: enqueue an item
// Blocks if buffer is full until space becomes available
// Returns 0 on success, -1 if buffer is closed
int bb_enqueue(bounded_buffer_t *bb, void *item);

// Consumer: dequeue an item
// Blocks if buffer is empty until item becomes available
// Returns item on success, NULL if buffer is closed and empty
void* bb_dequeue(bounded_buffer_t *bb);

// Try to enqueue without blocking
// Returns 0 on success, -1 if full or closed
int bb_try_enqueue(bounded_buffer_t *bb, void *item);

// Try to dequeue without blocking
// Returns item on success, NULL if empty or closed
void* bb_try_dequeue(bounded_buffer_t *bb);

// Close the buffer for new writes
// Producers get -1 on enqueue, consumers can still drain
void bb_close(bounded_buffer_t *bb);

// Get current number of items (for debugging/stats)
size_t bb_size(bounded_buffer_t *bb);

// Get capacity
size_t bb_capacity(bounded_buffer_t *bb);

// Destroy and free all resources
// Must be closed first, waits for buffer to drain
void bb_destroy(bounded_buffer_t *bb);

#endif // RING_BUFFER_H