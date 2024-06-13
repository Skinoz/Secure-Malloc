#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

// Forward declaration if not included in the header
void *init_meta_pool();

// Test creation pool de meta
Test(simple, my_malloc_valid_access) {
    size_t value = 10; // Allocate memory for 10 values
    int count = 0;
    int *ptr = (int *)my_malloc(value);
    printf("[ My_Malloc ] First Malloc ptr: %p\n", ptr);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful
    // verify that the memory is accessible
    for (size_t i = 0; i < value; ++i) {
        ptr[i] = i + 1;
        count++;
    }
    // Show value at ptr :
    for (size_t i = 0; i < value; ++i) {
        printf("[ My_Malloc ] Show ptr[%lu] = %d\n", i, ptr[i]);
    }
    cr_assert_eq(count, value, "Memory is not accessible"); // Verify that the memory is accessible

    int *ptr2 = (int *)my_malloc(value);
    printf("[ My_Malloc 2 part ] Second Malloc ptr: %p\n", ptr2);
    cr_assert_not_null(ptr2, "Allocation failed"); // Verify that the allocation is successful
    // verify that the memory is accessible
    for (size_t i = 0; i < value; ++i) {
        ptr2[i] = i + 1;
        count++;
    }
    // Show value at ptr :
    for (size_t i = 0; i < value; ++i) {
        printf("[ My_Malloc 2 part ] Show ptr[%lu] = %d\n", i, ptr2[i]);
    }
    cr_assert_eq(count, value+10, "Memory is not accessible"); // Verify that the memory is accessible

    my_free(ptr);
    my_free(ptr2);
}

Test(simple, my_malloc_multiple) {
    size_t value = 10;
    int *ptr = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr: %p\n", ptr);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful

    int *ptr2 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr2: %p\n", ptr2);
    cr_assert_not_null(ptr2, "Allocation failed"); // Verify that the allocation is successful

    int *ptr3 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr: %p\n", ptr);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful

    int *ptr4 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr2: %p\n", ptr2);
    cr_assert_not_null(ptr2, "Allocation failed"); // Verify that the allocation is successful

    int *ptr5 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr: %p\n", ptr);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful

    int *ptr6 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr2: %p\n", ptr2);
    cr_assert_not_null(ptr2, "Allocation failed"); // Verify that the allocation is successful

    int *ptr7 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr: %p\n", ptr);
    cr_assert_not_null(ptr, "Allocation failed"); // Verify that the allocation is successful

    int *ptr8 = (int *)my_malloc(value);
    printf("[ My_Malloc ] ptr2: %p\n", ptr2);
    cr_assert_not_null(ptr2, "Allocation failed"); // Verify that the allocation is successful


    my_free(ptr);
    my_free(ptr2);
    my_free(ptr3);
    my_free(ptr4);
    my_free(ptr5);
    my_free(ptr6);
    my_free(ptr7);
    my_free(ptr8);


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
}


Test(simple, my_calloc) {
    size_t nmemb = 5;
    size_t size = sizeof(int);

    int *ptr = (int *)my_calloc(nmemb, size); // Create an array of 5 integers
    cr_assert_not_null(ptr, "calloc allocation failed"); // Verify that the allocation is successful

    // Step 2: Set values to the allocated memory
    for (size_t i = 0; i < nmemb; ++i) {
        ptr[i] = 'A' + i; // Set values starting from 'A' to 'E'
    }

    // Step 3: Ensure memory is initialized correctly
    for (size_t i = 0; i < nmemb; ++i) {
        printf("[ My_Calloc ] ptr[%lu] = %c\n", i, ptr[i]); // Print the value set in the allocated memory
        cr_assert_eq(ptr[i], 'A' + i, "Incorrect value set in allocated memory"); // Verify that the memory is initialized correctly
    }
    printf("[ My_Calloc ] ptr: %p\n", ptr);

    my_free(ptr);
}


Test(simple, my_realloc_int) {
    size_t initial_size = 5 * sizeof(int);
    int *ptr = (int *)my_malloc(initial_size); // Allocate memory for 5 integers
    cr_assert_not_null(ptr, "Initial allocation failed");

    for (int i = 0; i < 5; ++i) {
        ptr[i] = i + 1;
        printf("[ My_Realloc ] ptr[%d] = %d\n", i, ptr[i]);
    }

    size_t new_size = 10 * sizeof(int);
    int *new_ptr = (int *)my_realloc(ptr, new_size);

    cr_assert_not_null(new_ptr, "Realloc allocation failed for 10 integers"); // Verify that the reallocation is successful

    for (int i = 0; i < 5; ++i) {
        cr_assert_eq(new_ptr[i], i + 1, "Content of original memory is not preserved"); // Verify that the content of the original memory is preserved
        printf("[ My_Realloc ] new_ptr[%d] = %d\n", i, new_ptr[i]);
    }

    // Step 6: Free the reallocated memory
    my_free(new_ptr);
}
