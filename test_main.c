/*
 * CS 551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "bitmap.c"
#include "stdlib.h"
#include "common.h"
#include "string.h"


/**
    Test 1: make sure expansion works
*/
void test_1() {
    mem_mngr_init();

    for (int i = 0; i < MEM_BATCH_SLOT_COUNT; i++) {
        int *x = mem_mngr_alloc(4);
    }

    // bit map is full at this point, time to trigger expansion
    printf("Before expansion:\n");
    mem_mngr_print_snapshot();
    int* ptr = mem_mngr_alloc(4);
    printf("After expansion:\n");
    mem_mngr_print_snapshot();

    mem_mngr_leave();
    printf("\n");
}

/**
    Make sure more than 8 allocations doesn't trigger expansion if previous allocs are freed
*/
void test_2() {
    mem_mngr_init();
    // allocate all memory
    int **x = calloc(MEM_BATCH_SLOT_COUNT, sizeof(int*));
    for (int i = 0; i < MEM_BATCH_SLOT_COUNT; i++) {
        x[i] = mem_mngr_alloc(4);
    }
    printf("All memory allocated:\n");
    mem_mngr_print_snapshot();

    // free up some space so expansion isn't necessary
    mem_mngr_free(x[6]);
    mem_mngr_free(x[2]);
    printf("After freeing slots 2 and 6:\n");
    mem_mngr_print_snapshot();

    // mem_mngr should alloc 2 before 6.
    printf("After allocating a new slot, slot 2 should be in use.\n");
    int* ptr8 = mem_mngr_alloc(4);
    mem_mngr_print_snapshot();

    mem_mngr_leave();
}

/**
    Test 3: make sure freeing works in expanded memory
*/
void test_3() {
    mem_mngr_init();

    int **x = calloc(MEM_BATCH_SLOT_COUNT << 1, sizeof(int*));
    for (int i = 0; i < (MEM_BATCH_SLOT_COUNT << 1); i++) {
        x[i] = mem_mngr_alloc(4);
    }
    // allocate all memory
    printf("All memory allocated (including expanded memory):\n");
    mem_mngr_print_snapshot();
    // free up some expanded memory

    printf("Freeing some slots in expanded memory (slots %d and %d):\n", MEM_BATCH_SLOT_COUNT + 2, MEM_BATCH_SLOT_COUNT + 5);
    mem_mngr_free(x[MEM_BATCH_SLOT_COUNT + 2]);
    mem_mngr_free(x[MEM_BATCH_SLOT_COUNT + 5]);
    mem_mngr_print_snapshot();

    mem_mngr_leave();
}

/**
    Double-free error test
*/
void test_4() {
    mem_mngr_init();

    int* ptr = mem_mngr_alloc(4);
    
    printf("Freeing pointer %p twice to induce double-free error:\n", ptr);
    mem_mngr_free(ptr);
    mem_mngr_free(ptr); // induce double free error

    mem_mngr_leave();
}

/**
    Unknown address error test
*/
void test_5() {
    mem_mngr_init();

    int* x = mem_mngr_alloc(4);
    int* ptr = x - 1;
    printf("Freeing unknown pointer %p to induce unknown address error:\n", ptr);
    mem_mngr_free(ptr); // induce unknown address error

    mem_mngr_leave();
}

int main(int argc, char * argv[])
{
    void (*tests[5])(void) = {&test_1, &test_2, &test_3, &test_4, &test_5};
    for (int i = 0; i < 5; i++) {
        printf("Running test %d:\n", i + 1);
        tests[i]();
        printf("\n");
    }
    return 0;
}
