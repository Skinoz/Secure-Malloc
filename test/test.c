#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

// Forward declaration if not included in the header
void *init_meta_pool();

// Test creation pool de meta
Test(simple, my_malloc_valid_access) {
    printf("---- START test my_malloc_valid_access ----\n");
    size_t value = 10; // Allocate memory for 10 values
    int count = 0;
    int *ptr = (int *)my_malloc(value);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful
    // verify that the memory is accessible
    for (size_t i = 0; i < value; ++i) {
        ptr[i] = i + 1;
        count++;
    }
    printf("[ My_Malloc ] ptr: %p\n", ptr); 
    // Show value at ptr :
    for (size_t i = 0; i < value; ++i) {
        printf("[ My_Malloc ] Show ptr[%lu] = %d\n", i, ptr[i]);
    }
    cr_assert_eq(count, value, "Memory is not accessible"); // Verify that the memory is accessible
    printf("---- END test my_malloc_valid_access ----\n");
}



Test(simple, my_free){
    size_t value = 10;
    void *ptr1 = my_malloc(value);
    printf("[ My_Free ] ptr1 at %p\n", ptr1);
    my_free(ptr1);

    void *ptr2 = my_malloc(value);
    printf("[ My_Free ] ptr2 at %p\n", ptr2);

    cr_assert_eq(ptr1, ptr2, "The address of ptr1 and ptr2 should be the same"); // Logically the address should be the same because the memory is freed and reallocated
    my_free(ptr2);
    void *ptr3 = my_malloc(value); 
    cr_assert_eq(ptr2, ptr3, "The address of ptr2 and ptr3 should be the same"); // Logically the address should be the same because the memory is freed and reallocated
    printf("[ My_Free ] ptr3 at %p\n", ptr3);
}

/*
Test(simple, my_calloc) {
    // Step 1: Allocate memory using my_calloc
    size_t nmemb = 5;
    size_t size = sizeof(int); // Ensure that the size is not too small
    int *ptr = (int *)my_calloc(nmemb, size);
    cr_assert_not_null(ptr, "calloc allocation failed");

    // Step 2: Set values to the allocated memory
    for (size_t i = 0; i < nmemb; ++i) {
        ptr[i] = 'A' + i; // Set values starting from 'A'
    }

    // Step 3: Ensure memory is initialized correctly
    for (size_t i = 0; i < nmemb; ++i) {
        printf("[ My_Calloc ] ptr[%lu] = %c\n", i, ptr[i]);
        cr_assert_eq(ptr[i], 'A' + i, "Incorrect value set in allocated memory");
    }
    printf("[ My_Calloc ] ptr: %p\n", ptr);

    // Step 3: Free the allocated memory
    my_free(ptr);

    printf("---- END test my_calloc ----\n");
}
*/

Test(simple, my_realloc_int) {
    printf("---- START test my_realloc_int ----\n");

    // Step 1: Allocate memory for an array of 5 integers using my_malloc
    size_t initial_size = 5 * sizeof(int);
    int *ptr = (int *)my_malloc(initial_size);
    cr_assert_not_null(ptr, "Initial allocation failed");

    // Step 2: Set initial values to the allocated memory
    for (int i = 0; i < 5; ++i) {
        ptr[i] = i + 1;
    }

    // Step 3: Try to realloc memory for an array of 6 integers using my_realloc
    size_t new_size = 6 * sizeof(int);
    int *new_ptr = (int *)my_realloc(ptr, new_size);

    // Step 4: Reallocate memory for an array of 10 integers using my_realloc
    new_size = 10 * sizeof(int);
    new_ptr = (int *)my_realloc(ptr, new_size);
    cr_assert_not_null(new_ptr, "Realloc allocation failed for 10 integers");

    // Step 5: Ensure content is preserved and memory is resized
    for (int i = 0; i < 5; ++i) {
        cr_assert_eq(new_ptr[i], i + 1, "Content of original memory is not preserved");
    }

    // Step 6: Free the reallocated memory
    my_free(new_ptr);

    printf("---- END test my_realloc_int ----\n");
}