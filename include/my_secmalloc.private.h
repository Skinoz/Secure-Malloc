#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

#include "my_secmalloc.h"
#include <stdint.h> // uint64_t
#include <stddef.h> // size_t

struct metadata_t {
    struct metadata_t   *next;
    void                *data_ptr;
    size_t              size;
    int                 isFree; // 0 = BUSY // 1 = FREE
};

// Function prototypes for memory management
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);
void *init_meta_pool();
void *init_data_pool();

#endif // _SECMALLOC_PRIVATE_H
