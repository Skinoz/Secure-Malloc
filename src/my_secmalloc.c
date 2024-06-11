// My_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void *meta_pool = NULL; // commence n'importe ou
size_t meta_pool_size = sizeof(struct metadata_t) * 1000; // on va dire que on peut faire 1000 malloc

void *data_pool = NULL; // commence juste apres le metadata_pool
size_t data_pool_size = 4096;

// initalisation/creation du pool de meta
void    *init_meta_pool()
{
    if (meta_pool == NULL)
    {
        meta_pool = mmap(NULL, meta_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    }
    return meta_pool;
}

// initalisation/creation du pool de data
void    *init_data_pool()
{
    if (data_pool == NULL)
    {
        data_pool = mmap(meta_pool + meta_pool_size, data_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    }
    return data_pool;
}

void    *get_free_chunk(size_t s)
{
    if (meta_pool == NULL)
        meta_pool = init_meta_pool();
    if (data_pool == NULL)
        data_pool = init_data_pool();
    // ici faire un for qui parcours le metapool
    for (struct metadata_t *item = meta_pool;(size_t)item < (size_t)meta_pool + meta_pool_size; item = (struct metadata_t *)((size_t)item + sizeof(struct metadata_t)))
    {
        if (item->isFree == 1 && item->size > s)
        {
            return item->data_ptr;
        }
    }
    return NULL;
}

void    *my_malloc(size_t size)
{
    (void) size;
    //void *ptr;
    // get free chunk
    struct chunk *ch = get_free_chunk(size);
    
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
