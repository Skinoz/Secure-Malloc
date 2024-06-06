// my_secmalloc.private.h

#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

#include "my_secmalloc.h"
#include <stdint.h> // uint64_t
#include <stddef.h> // size_t

// Enumeration to represent the state of a memory block
typedef enum {
    FREE,  // Block is Free
    BUSY   // Block is Busy
} State;

// Structure representing the metadata of a memory block
typedef struct _meta_pool {
    State state;             // FREE || BUSY
    Data_pool* ptr;          // Pointer to data_pool (block)
    struct _meta_pool *next; // Pointer to the next Meta_Data in meta_pool
    struct _meta_pool *prev; // Pointer to the previous Meta_Data in meta_pool
} Meta_pool;

// Structure representing a data pool with linked list connections
typedef struct _data_pool {
    uint64_t canary;        // Value to define integrity of each block
    size_t size;            // Size of the block
} Data_pool;

// Function prototypes for memory management
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);

#endif // _SECMALLOC_PRIVATE_H