// My_secmalloc.c
#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

struct metadata_t *meta_pool = NULL; // commence n'importe ou
size_t meta_pool_size = sizeof(struct metadata_t) * 1000; // on va dire que on peut faire 1000 malloc

void *data_wptr = NULL; // commence juste apres le metadata_pool
size_t data_pool_size = 4096;

// initalisation/creation du pool de meta
struct metadata_t    *init_meta_pool()
{
    if (meta_pool == NULL)
    {
        meta_pool = (struct metadata_t*)mmap(NULL, meta_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        data_wptr = mmap(meta_pool + meta_pool_size, data_pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        
        meta_pool->next = NULL;
        meta_pool->data_ptr = data_wptr;
        meta_pool->size = data_pool_size;
        meta_pool->isFree = 1;
    }
    return meta_pool;
}

struct metadata_t    *get_free_chunk(size_t s)
{
    if (meta_pool == NULL)
    {
        meta_pool = init_meta_pool();
        printf("## STRUCTURE METADATA_T METAPOOL INIT A L'ADRESSE = %p\n", meta_pool);
        printf("## NEXT %p\n", meta_pool->next);
        printf("## DATA_PTR %p\n", meta_pool->data_ptr);
        printf("## SIZE %zu\n", meta_pool->size);
        printf("## ISFREE? %d\n", meta_pool->isFree);
    }
    
    // ici faire un for qui parcours le metapool
    for (struct metadata_t *item = meta_pool;(size_t)item < (size_t)meta_pool + meta_pool_size; item = (struct metadata_t *)((size_t)item + sizeof(struct metadata_t)))
    {
        printf("item = %p\n", item);
        printf("1 iteration\n");
        if (item->isFree == 1 && item->size > s) // si le chunk est libre et que sa taille est plus grande que celle demandee
        {

            printf("RENTRE DANS LE IF -- CHUNK EST DISPO\n");
            printf("data ptr 1 = %p\n", item->data_ptr);
            struct metadata_t *next_meta = (struct metadata_t *)((char *)item + sizeof(struct metadata_t));
            next_meta->next = NULL;
            next_meta->data_ptr = data_wptr + s;
            next_meta->size = item->size - s;
            next_meta->isFree = 1;
            printf("next_meta next = %p\n", next_meta->next);
            printf("next_meta data_ptr = %p\n", next_meta->data_ptr);
            printf("next_meta size = %ld\n", next_meta->size);
            printf("next_meta isfree? = %d\n", next_meta->isFree);
            item->next = next_meta;
            item->isFree = 0;
            printf("data ptr 2 = %p\n", item->data_ptr);
            
            return item;
        }        
        else if (item->next == NULL)
        {
            printf("aucun chunk de la taille demandee a ete trouve, entension du mmap.......\n");
            printf("data_wptr = %p\n", data_wptr);
            data_wptr = mremap(data_wptr, data_pool_size, data_pool_size + s, MREMAP_MAYMOVE);
            printf("data_wptr after remap = %p\n", data_wptr);
            // augmentation du datapool
            data_pool_size = data_pool_size + s;
            get_free_chunk(s);
            // recurvisite infinie , find stop point
            return item;
        }
    }
    return NULL;
}

void    *my_malloc(size_t size)
{
    (void) size;
    //void *ptr;
    // get free chunk
    printf("OK\n");
    struct metadata_t *ch = get_free_chunk(size);
    printf("## CHUNK TROUVE !!!! == %p\n", ch);
    // il faut mtn passer le chunk a free et dire que le next est apres

    //next_meta->next = NULL;
    //next_meta->data_ptr = data_wptr + size;
    //next_meta->isFree = 1;
    return ch->data_ptr;
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
