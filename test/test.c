#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

// Forward declaration if not included in the header
void *init_meta_pool();

// Test creation pool de meta
Test(simple, meta_pool_init) {
    printf("---- START test my_malloc ----\n");
    size_t value = 10;
    void *ptr = my_malloc(value);
    void *ptr2 = my_malloc(value);
    void *ptr3 = my_malloc(value);
    void *ptr4 = my_malloc(value);
    void *ptr5 = my_malloc(value);
    void *ptr6 = my_malloc(value);
    void *ptr7 = my_malloc(value);
    printf("ptr : %p\n", ptr);
    printf("ptr2 : %p\n", ptr2);
    printf("ptr3 : %p\n", ptr3);
    printf("ptr4 : %p\n", ptr4);
    printf("ptr5 : %p\n", ptr5);
    printf("ptr6 : %p\n", ptr6);
    printf("ptr7 : %p\n", ptr7);
    printf("---- END test my_malloc ----\n");
}


Test(simple, my_free){
    printf("---- START test my_free ----\n");
    size_t value = 10;
    void *ptr1 = my_malloc(value);
    my_free(ptr1);

    void *ptr2 = my_malloc(value);
    printf("ptr2 at %p\n", ptr2);

    void *ptr3 = my_malloc(value);
    printf("ptr2 at %p\n", ptr3);

    printf("---- END test my_free ----\n");
}