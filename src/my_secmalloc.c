// my_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void *meta_pool = NULL; // Initialize at any position
size_t meta_pool_size = sizeof(struct metadata_t) * 1000; // 1000 metadata_t structures

void *data_pool = NULL; // Initialize at any position
size_t data_pool_size = 4096; // Default data pool size

struct metadata_t *meta_information = NULL; // Metadata information

// Initialize and return the address of the first metadata structure
void *init_meta_pool(void) {
    if (meta_pool == NULL) {
        meta_pool = mmap(NULL, meta_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        
        // If mmap fails
        if (meta_pool == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }

        meta_information = (struct metadata_t *)meta_pool; // Initialize meta_information with the start of meta_pool

        // Initialize the first metadata structure
        meta_information->next = NULL;
        meta_information->data_ptr = NULL; 
        meta_information->size = data_pool_size;
        meta_information->isFree = 1;

        printf("meta_pool start address %p\n", meta_pool); // Print the start address of the meta_pool
    } else {
        printf("meta_pool already exists\n");
        exit(EXIT_FAILURE); // Exit if meta_pool already exists
    }
    return meta_pool; // Return the start address of the meta_pool
}

// Initialization/creation of the data pool
void *init_data_pool(void) {
    if (data_pool == NULL) {
        data_pool = mmap((void *)((char *)meta_pool + meta_pool_size), data_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        // If mmap fails
        if (data_pool == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("data_pool start address %p\n", data_pool);

    return data_pool;
}

void *get_free_chunk(size_t s) {
    // If meta_pool is not defined
    if (meta_pool == NULL) {
        printf("MetaPool is Null\n");
        meta_pool = init_meta_pool();
    }
    // If data_pool is not defined
    if (data_pool == NULL) {
        printf("DataPool is Null\n");
        data_pool = init_data_pool();
        meta_information->data_ptr = data_pool; // Set pointer to meta information
    }

    // Iterate through the metadata pool
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        // if the block is free and the size is greater than or equal to the requested size
        if (item->isFree == 1 && item->size >= s) { 
            item->isFree = 0; // Mark as not free
            return item->data_ptr; // Return the pointer to the data block associated with the metadata
        }
    }
    // TODO: Implement remap if needed
    return NULL;
}

void *my_malloc(size_t size) {
    size_t total_size = sizeof(struct metadata_t) + size;
    void *ch = get_free_chunk(total_size);

    if (ch == NULL) {
        // Allocate a new block if no free chunk is available
        struct metadata_t *new_meta = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        if (new_meta == MAP_FAILED) {
            perror("mmap failed");
            return NULL;
        }

        // Initialize the new metadata structure
        new_meta->next = meta_information;
        new_meta->data_ptr = (void *)((char *)new_meta + sizeof(struct metadata_t));
        new_meta->size = size;
        new_meta->isFree = 0;

        meta_information = new_meta; // Add new metadata to the list
        ch = new_meta->data_ptr;
    }

    printf("Allocated chunk at %p\n", ch);
    return ch;
}

void my_free(void *ptr) {
    // Find the metadata associated with the given pointer
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        if (item->data_ptr == ptr) {
            item->isFree = 1; // Mark the block as free
            return;
        }
    }
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
