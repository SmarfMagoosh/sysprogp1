/*
 * CS 551 Project "Memory manager".
 * This file needs to be turned in.	
 */
#include "stdlib.h"
#include "memory_manager.h"

static STRU_MEM_LIST* mem_pool = NULL;

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void)
{
    STRU_MEM_LIST* mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool; // Get the first memory list
    while(NULL != mem_list)
    {
        STRU_MEM_BATCH* mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
                   mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch)
        {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager with 8 bytes slot size mem_list.
 * Initialize this list with 1 batch of slots.
 */
void mem_mngr_init(void) 
{
    // allocate a new list
    STRU_MEM_LIST* next_pool = malloc(sizeof(STRU_MEM_LIST));
    if (!next_pool) {
        return;
    }

    // allocate a batch for it
    STRU_MEM_BATCH* batch = malloc(sizeof(STRU_MEM_BATCH));
    if (!batch) {
        free(next_pool);
        return;
    }

    // allocate all the slots for the batch
    batch -> batch_mem = calloc(MEM_BATCH_SLOT_COUNT, MEM_ALIGNMENT_BOUNDARY);
    if (!(batch -> batch_mem)) {
        free(batch);
        free(next_pool);
        return;
    }
    batch -> next_batch = NULL;

    // set up the bitmap
    int bm_size = (MEM_BATCH_SLOT_COUNT + 7) / 8;
    unsigned char* bm = calloc(bm_size, sizeof(unsigned char));
    if (!bm) {
        free(batch -> batch_mem);
        free(batch);
        free(next_pool);
        return;
    }
    memset(bm, 0xFF, bm_size);

    // assign the fields of the list
    next_pool -> slot_size = MEM_ALIGNMENT_BOUNDARY;
    next_pool -> batch_count = 1;
    next_pool -> free_slots_bitmap = bm;
    next_pool -> bitmap_size = bm_size;
    next_pool -> first_batch = batch;
    
    // prepend to the memory pool
    next_pool -> next_list = mem_pool;

    // update the head pointer to the new head
    mem_pool = next_pool;
}

// Helper function for freeing a batch
static void free_batch(STRU_MEM_BATCH* batch) 
{
    // no double-freeing!
    if (!batch) {
        return;
    }
    free(batch -> batch_mem);
    free_batch(batch -> next_batch);
    free(batch);
    
}

// Helper function for freeing a list
static void free_list(STRU_MEM_LIST* list) 
{
    // no double-freeing!
    if (!list) {
        return;
    }
    free_list(list -> next_list);
    free(list -> free_slots_bitmap);
    free_batch(list -> first_batch);
    free(list);
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void) 
{
	if (!mem_pool) {
        return;
    }
    free_list(mem_pool);
    mem_pool = NULL;
}

/*
 * Allocate a chunk of memory 	
 * @param size: size in bytes to be allocated
 * @return: the pointer to the allocated memory slot
 */
void* mem_mngr_alloc(size_t size)
{
    // assume the original size is <= 8, so aligned size = 8
    size = SLOT_ALLINED_SIZE(size);

    // set up references to the memory list and batch
    STRU_MEM_LIST* list = mem_pool;
    STRU_MEM_BATCH* batch = list -> first_batch;

    // try to find a slot to allocate
    int slot = bitmap_find_first_bit(list -> free_slots_bitmap, list -> bitmap_size, 1);

    // if no slot was found we need to expand
    if (slot == BITMAP_OP_NOT_FOUND) {
        // add a new batch to the list
        list -> batch_count += 1;
        STRU_MEM_BATCH* new_batch = malloc(sizeof(STRU_MEM_BATCH));
        if (!new_batch) {
            return NULL;
        }

        // allocate and assign variables for the new batch
        new_batch -> batch_mem = calloc(MEM_BATCH_SLOT_COUNT, MEM_ALIGNMENT_BOUNDARY);
        if (!(new_batch -> batch_mem)) {
            free(new_batch);
            return NULL;
        }
        new_batch -> next_batch = NULL;
        STRU_MEM_BATCH* curr = batch;

        // append it to the end of the batch list
        while (curr -> next_batch) {
            curr = curr -> next_batch;
        }
        curr -> next_batch = new_batch;

        // rework the bitmap
        int prev_bm_size = list -> bitmap_size;
        int additional_bytes = (MEM_BATCH_SLOT_COUNT + 7) / 8;
        list -> bitmap_size += additional_bytes;
        list -> free_slots_bitmap = realloc(list -> free_slots_bitmap, list -> bitmap_size);
        memset(list -> free_slots_bitmap + prev_bm_size, 0xFF, additional_bytes);

        // try again (recursion is fine here since a slot will be found next iteration)
        return mem_mngr_alloc(size);
    }
    // mark the slot as used
    int test = bitmap_clear_bit(list -> free_slots_bitmap, list -> bitmap_size, slot);

    // find the slots batch and offset
    while (slot > MEM_BATCH_SLOT_COUNT) {
        batch = batch -> next_batch;
        slot -= MEM_BATCH_SLOT_COUNT;
    }
    
    // return the address if no error
    if (test == BITMAP_OP_ERROR) {
        return NULL;
    } else {
        return (slot * list -> slot_size) + ((unsigned char*) batch -> batch_mem);
    }
}

/*
 * Free a chunk of memory pointed by ptr
 * Print out any error messages
 * @param: the pointer to the allocated memory slot
 */
void mem_mngr_free(void* ptr)
{
    // set up references to the memory list and current batch
    STRU_MEM_LIST* list = mem_pool;
    STRU_MEM_BATCH* batch = NULL;

    // search through all lists
    while (list) {
        int slot_index = 0;
        batch = list -> first_batch;
        // search through all batches in the list
        while (batch) {
            // search through all slots in the batch
            for (int i = 0; i < MEM_BATCH_SLOT_COUNT; i++) {
                void* slot_addr = (i * list -> slot_size) + ((unsigned char*) batch -> batch_mem);
                // if we have found the pointer, make sure it is allocated and free it
                if (slot_addr == ptr) {
                    if (bitmap_bit_is_set(list -> free_slots_bitmap, list -> bitmap_size, slot_index)) {
                        printf("ERROR: cannot free unallocated memory slot %p\n", ptr);
                        return;
                    }
                    bitmap_set_bit(list -> free_slots_bitmap, list -> bitmap_size, slot_index);
                    return;
                }
                slot_index++;
            }
            batch = batch -> next_batch;
        }
        list = list -> next_list;
    }
    // if pointer can't be found something went wrong...
    printf("ERROR: pointer %p not found in memory manager.\n", ptr);
    return;
}