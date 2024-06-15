// my_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define POOL_SIZE 1000 // Set size of meta_pool
#define CHUNK_SIZE 4096 // Default chunk size

void *meta_pool = NULL; // Initialize at any position
size_t meta_pool_size = sizeof(struct metadata_t) * POOL_SIZE; // 1000 metadata_t structures

void *data_pool = NULL; // Initialize at any position
size_t data_pool_size = 4096; // Default data pool size

struct metadata_t *meta_information = NULL; // Metadata information

void log_action(const char *format, ...) {
    FILE *log_file = fopen("secure_malloc.log", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    // Get the current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Format the timestamp
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // Start variadic arguments processing
    va_list args;
    va_start(args, format);

    // Use vsnprintf to determine the size of the formatted string
    int size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
    va_end(args);

    // Allocate space for the formatted string
    char *message = alloca(size);

    va_start(args, format);
    vsnprintf(message, size, format, args);
    va_end(args);

    // Write the timestamp and the message to the log file
    fprintf(log_file, "[%s] %s\n", time_str, message);
    fclose(log_file);
}

// Initialize and return the address of the first metadata structure
void *init_meta_pool(void) {
    log_action("Initialize metadata pool");
    if (meta_pool == NULL) {
        meta_pool = mmap(NULL, meta_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0); // Init pool of meta        
        // If mmap fails
        if (meta_pool == MAP_FAILED) {
            perror("mmap failed");
            log_action("Failed to create metadata pool");
            exit(EXIT_FAILURE);
        }

        meta_information = (struct metadata_t *)meta_pool; // Give allocation in memory for meta_information

        // Initialize with 1000 structure of meta_data with default value :
        for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
            item->next = (struct metadata_t *)((char *)item + sizeof(struct metadata_t));
            item->data_ptr = NULL; 
            item->size = data_pool_size;
            item->isFree = 1;
        }

        log_action("meta_pool initialized at %p", meta_pool);

        // printf("meta_pool start address %p\n", meta_pool); // Print the start address of the meta_pool
    } else {
        log_action("meta_pool already exists");
        // printf("meta_pool already exists\n");
        exit(EXIT_FAILURE); // Exit if meta_pool already exists
    }
    return meta_pool; // Return the start address of the meta_pool
}

struct metadata_t *get_free_metadata() {
    int count = 0;
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item = (struct metadata_t *)((char *)item + sizeof(struct metadata_t))) {
        if (item->isFree == 1 && item->data_ptr == NULL) {
            return item; // Return the pointer to the free meta_data block in metadatapool
        }
        count = count+1;
        // printf("[ Count : %d ] Is Free at %p ? : %d | data_ptr at : %p\n", count,item, item->isFree, item->data_ptr);
        // printf("meta_item %d\n", count); How many meta_data structure of meta_data i have
        log_action("[ %d ] Pointer %p | Next is %p | Free %d | data_ptr : %p | size : %d\n", count ,item,item->next,item->isFree,item->data_ptr, item->size);
    }
    log_action("No free metadata slot available"); // Log the error
    return NULL;
}

void *my_malloc(size_t size) {
    log_action("malloc requested for size %zu", size);
    if (meta_pool == NULL) {
        meta_pool = init_meta_pool();
    }
    
    struct metadata_t *new_meta = get_free_metadata();
    if (new_meta == NULL) {
        perror("No free metadata slot available");
        return NULL;
    }

    void *new_data_block = mmap(NULL, size + sizeof(struct data_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0); // Size + data_info
    if (new_data_block == MAP_FAILED) {
        log_action("mmap failed for new data block"); // Log the error
        perror("mmap failed");
        return NULL;
    }

    // Set canary value
    struct data_t *data_info = (struct data_t *)new_data_block; // Cast new_data_block as data_t 
    data_info->canary = 0xdeadbeef; // Canary value

    new_meta->data_ptr = (void *)((char *)new_data_block + sizeof(struct data_t)); // Target data
    new_meta->size = size; // Add size of data
    new_meta->isFree = 0; // Set busy

    log_action("malloc successful: allocated %zu bytes at %p", size, new_meta->data_ptr); // Log the successful allocation
    return new_meta->data_ptr; // Return pointer of data
}


void my_free(void *ptr) {
    log_action("free requested for pointer %p", ptr); // Log the free request
    for (struct metadata_t *item = meta_information; (char *)item < (char *)meta_pool + meta_pool_size; item++) { // Foreach all meta struct
        if (item->data_ptr == ptr) { // Find ptr are set in argument
            struct data_t *data_info = (struct data_t *)((char *)ptr - sizeof(struct data_t));
            if (data_info->canary != 0xdeadbeef) { // Check if canary are not altered
                log_action("Buffer overflow detected for pointer %p", ptr);
                perror("Buffer overflow detected");
                return;
            }
            munmap(data_info, item->size + sizeof(struct data_t)); // Deallocate at data at pointer and give size + structure
            item->data_ptr = NULL; // Delete data_ptr of meta
            item->size = 0; // Set size to 0 because it doesnt allocate data
            item->isFree = 1; // Set is Free
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
    // If ptr is NULL, use my_malloc
    if (ptr == NULL) {
        return my_malloc(size);
    }

    // If size is zero and ptr is not NULL, use Free
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

    struct data_t *data_info = (struct data_t *)((char *)ptr - sizeof(struct data_t)); // Target start of data block
    size_t current_size = meta_ptr->size + sizeof(struct data_t);

    // Resize the memory region using mremap
    void *new_block = mremap(data_info, current_size, size + sizeof(struct data_t), MREMAP_MAYMOVE); // Resize block with new size
    if (new_block == MAP_FAILED) {
        perror("mremap failed");
        return NULL;
    }

    // Update metadata for the new data block
    data_info = (struct data_t *)new_block;
    meta_ptr->data_ptr = (void *)((char *)new_block + sizeof(struct data_t));
    meta_ptr->size = size;

    return meta_ptr->data_ptr;
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
