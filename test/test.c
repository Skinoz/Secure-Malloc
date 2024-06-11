#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

// Forward declaration if not included in the header
void *init_meta_pool();

// Test creation pool de meta
Test(simple, meta_pool_init) {
    printf("---- Start test my_malloc ----\n");
    size_t value = 10;
    void *ptr = my_malloc(value);
    printf("ptr : %p\n", ptr);
}


Test(simple, my_free){
    printf("---- Start test my_free ----\n");
    size_t value = 10;
    void *ptr = my_malloc(value);
    printf("ptr : %p\n", ptr);
    my_free(ptr);
    void *ptr2 = my_malloc(value);
    printf("ptr : %p\n", ptr2);
    cr_assert(ptr2 == ptr);
}