// My_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void *meta_pool = NULL; /// Initialize at any position
size_t meta_pool_size = sizeof(struct metadata_t) * 1000; // 1000 metadata_t structure

void *data_pool = NULL; // Initialize at any position
size_t data_pool_size = 4096; // Default data pool size

struct metadata_t *meta_information = NULL; // Metadata information

/// Initialize and return address of the first metadata structure
void *init_meta_pool() {
    if (meta_pool == NULL) {
        meta_pool = mmap(NULL, meta_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        
        // If mmap fail
        if (meta_pool == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }

        meta_information = (struct metadata_t *)meta_pool; // Create meta_information structure and initialize with start of meta_pool

        // Initialize the first metadata structure, Can use it to store information
        meta_information->next = NULL; // Pointer to the next metadata structure (Null because it's the first one)
        meta_information->data_ptr = NULL; // Pointer to the data block (They initialize it on init_data_pool())
        meta_information->size = 4096; // Default data pool size
        meta_information->isFree = 1; // Free block

        // printf("meta_pool start adresse %p\n", meta_pool); // Print the start address of the meta_pool
    } else {
        printf("meta_pool already exists\n");
        exit(EXIT_FAILURE); // Exit if meta_pool already exists
    }
    return meta_pool; // Return the start address of the meta_pool
}

// initalisation/creation du pool de data
void    *init_data_pool()
{
    if (data_pool == NULL)
    {
        data_pool = mmap(meta_pool + meta_pool_size, data_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    }
    void *ptr = data_pool;
    printf("data_pool start adresse %p\n", ptr);

    return data_pool;
}

void    *get_free_chunk(size_t s) {
    // If meta_pool are not define
    if (meta_pool == NULL){
        printf("MetaPool is Null\n");
        meta_pool = init_meta_pool();
    }
    // If data are not define
    if (data_pool == NULL){
        printf("DataPool is Null\n");
        data_pool = init_data_pool();
        meta_information->data_ptr = data_pool; // Set pointer to meta information
    }

    // Iterate through the metadata pool
    for (struct metadata_t *item = meta_information; (size_t)item < (size_t)meta_pool + meta_pool_size; item = (struct metadata_t *)((size_t)item + sizeof(struct metadata_t))) {
        // printf("item %ld\n", item->size); Information about the size of the data block 
        // printf("size : %ld\n", s); Information about the size of request function

        // if the block is free and the size is greater than the requested size
        if (item->isFree == 1 && item->size > s) { 
            return item->data_ptr; // Return the pointer to the data block associated with the metadata
        }
    }
    // TODO : Implement remmap ?
    return NULL;
}


void    *my_malloc(size_t size)
{
    //void *ptr;
    // get free chunk
    void *ch = get_free_chunk(size);
    
    printf("chunk %p\n", ch);

    return ch;
}
void    my_free(void *ptr)
{
    (void) ptr;
}
void    *my_calloc(size_t nmemb, size_t size)
{
    (void) nmemb;
    (void) size;
    return NULL;
}

void    *my_realloc(void *ptr, size_t size)
{
    (void) ptr;
    (void) size;
    return NULL;

}

#ifdef DYNAMIC
void    *malloc(size_t size)
{
    return my_malloc(size);
}
void    free(void *ptr)
{
    my_free(ptr);
}
void    *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

void    *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);

}

#endif
