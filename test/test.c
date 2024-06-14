#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

/* 
// test creation pool de meta
Test(simple, init_meta_pool) {
    void *ptr = my_malloc(32);
    cr_assert(ptr != NULL, "Fail");
    void *ptr2 = my_malloc(1024);
    cr_assert(ptr2 != NULL, "Fail");
    void *ptr3 = my_malloc(4096);
    cr_assert(ptr3 != NULL, "Fail");
} */

// test free
Test(simple, free) {
    void *ptr = my_malloc(32);
    printf("ptr addr = %p\n", ptr);
    my_free(ptr);
    //cr_assert(ptr != NULL, "Fail");
}

/* Test(simple, malloc2) {
    void *ptr1 = my_malloc(8);
    cr_assert(ptr1 != NULL, "Failed to malloc2");
    void *ptr2 = my_malloc(8);
    cr_assert((size_t)ptr2 == (size_t)ptr1 + 12 + (sizeof(struct chunk)), "Failed to alloc :");
} */