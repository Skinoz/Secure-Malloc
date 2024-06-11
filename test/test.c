#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

// Forward declaration if not included in the header
void *init_meta_pool();

// Test creation pool de meta
Test(simple, meta_pool_init) {
    size_t value = 10;
    void *ptr = my_malloc(value);
    printf("ptr : %p\n", ptr);
}
