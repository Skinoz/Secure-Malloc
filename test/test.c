#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>


// test creation pool de meta
Test(simple, meta_pool_init) {
    void *ptr = my_malloc(32);
    cr_assert(ptr != NULL, "Failed to init meta_pool");
}

Test(simple, malloc2) {
    void *ptr1 = my_malloc(8);
    cr_assert(ptr1 != NULL, "Failed to malloc2");
    void *ptr2 = my_malloc(8);
    cr_assert((size_t)ptr2 == (size_t)ptr1 + 12 + (sizeof(struct chunk)), "Failed to alloc :");
}