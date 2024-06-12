// my_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define POOL_SIZE 1000
#define CHUNK_SIZE 4096

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

        meta_information = (struct metadata_t *)meta_pool; // Give allocation in memory for meta_information

        // Initialize with 1000 structure of meta_data with default value :
        for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
            item->data_ptr = NULL; 
            item->size = data_pool_size;
            item->isFree = 1;
        }

        // printf("meta_pool start address %p\n", meta_pool); // Print the start address of the meta_pool
    } else {
        // printf("meta_pool already exists\n");
        exit(EXIT_FAILURE); // Exit if meta_pool already exists
    }
    return meta_pool; // Return the start address of the meta_pool
}

// Initialization/creation of the data pool
void *init_data_pool(void) {
    if (data_pool == NULL) {
        data_pool = mmap((void *)((char *)meta_pool + meta_pool_size), data_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0); // Start address of data_pool is after the meta_pool

        // If mmap fails
        if (data_pool == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
    }
    // printf("data_pool start address %p\n", data_pool);

    return data_pool;
}

void *get_free_chunk(size_t s) {
    // Iterate through the metadata pool
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        // if the block is free and the size is greater than or equal to the requested size
        if (item->isFree == 1 && item->size >= s) { 
            return item->data_ptr; // Return the pointer to the data block associated with the metadata
        }
    }
    // TODO: Use realoc when no free chunk is found
    return NULL;
}

struct metadata_t *get_free_metadata() {
    int count = 0;
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        if (item->isFree == 1 && item->data_ptr == NULL) {
            return item; // Return the pointer to the free meta_data block in metadatapool
        }
        count++;
        // printf("[ Count : %d ] Is Free at %p ? : %d | data_ptr at : %p\n", count,item, item->isFree, item->data_ptr);
        // printf("meta_item %d\n", count); How many meta_data structure of meta_data i have
    }
    return NULL;
}

void *my_malloc(size_t size) {
    // TODO : Rezise data_pool if no free chunk is found
    // TODO : Add canary to detect buffer overflow
    if (meta_pool == NULL) {
        // printf("MetaPool is Null\n");
        meta_pool = init_meta_pool();
    }
    
    if (data_pool == NULL) {
        // printf("DataPool is Null\n");
        data_pool = init_data_pool();
        meta_information->data_ptr = data_pool; // Set pointer to meta information
    }
    
    // Find a free metadata slot
    struct metadata_t *new_meta = get_free_metadata();
    if (new_meta == NULL) {
        perror("No free metadata slot available");
        return NULL;
    }
    
    // Find a free data block
    void *ch = get_free_chunk(size);
    if (ch == NULL) {
        perror("Error at get free chunk");
        return NULL;
    }

    // Allocate memory for the new data block
    void *new_data_block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (new_data_block == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }

    // Update metadata for the new data block
    new_meta->data_ptr = new_data_block;
    new_meta->size = size;
    new_meta->isFree = 0;

    // printf("[ NEW ALLOCATION ] Is Free at %p ? : %d | data_ptr at : %p\n",new_meta, new_meta->isFree, new_meta->data_ptr);
    return new_data_block;
}


void my_free(void *ptr) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (meta_information[i].data_ptr == ptr) {
            munmap(ptr, meta_information[i].size); // Libération de la mémoire allouée pour ce bloc
            meta_information[i].data_ptr = NULL;
            meta_information[i].size = 0;
            meta_information[i].isFree = 1;
            return;
        }
    }
    perror("Metadata not found for pointer in free");
}

void *my_calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size; // Calculate the total size of memory to allocate
    void *ptr = my_malloc(total_size); // Allocate memory using my_malloc
    if (ptr != NULL) {
        // If allocation was successful, set all memory to zero
        memset(ptr, 0, total_size);
    }
    return ptr;
}


void *my_realloc(void *ptr, size_t size) {
    // If ptr is NULL, it behaves like malloc(size)
    if (ptr == NULL) {
        return my_malloc(size);
    }

    // If size is zero and ptr is not NULL, it behaves like free(ptr)
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    // Find the metadata associated with the given pointer
    struct metadata_t *meta_ptr = NULL;
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        if (item->data_ptr == ptr) {
            meta_ptr = item;
            break;
        }
    }

    // If metadata associated with the given pointer is not found
    if (meta_ptr == NULL) {
        perror("Metadata not found for pointer in realloc");
        return NULL;
    }

    // Resize the memory region using mremap
    void *new_ptr = mremap(ptr, meta_ptr->size, size, MREMAP_MAYMOVE);
    if (new_ptr == MAP_FAILED) {
        perror("mremap failed");
        return NULL;
    }

    // Update metadata for the new data block
    meta_ptr->data_ptr = new_ptr;
    meta_ptr->size = size;

    return new_ptr;
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
