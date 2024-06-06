// My_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void    *my_malloc(size_t size)
{
   // Allocate memory for Data_pool and the actual data block using mmap
    Data_pool *new_data_pool = (Data_pool *)mmap(
        NULL, 
        sizeof(Data_pool) + size + sizeof(uint64_t), // Size of Data_pool, size of allocated block and size of canary
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0
    );

    if (new_data_pool == MAP_FAILED) return NULL;

    return NULL;
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
