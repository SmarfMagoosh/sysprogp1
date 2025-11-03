/*
 * CS 551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "bitmap.c"
#include "stdlib.h"
#include "common.h"
#include "string.h"

char* int_to_bool(int val) {
    if (val == BITMAP_OP_ERROR) {
        return "ERROR";
    } else if (val == BITMAP_OP_NOT_FOUND) {
        return "NOT_FOUND";
    }
    return val ? "true" : "false";
}

void print_res(int exp) {
    printf("%s\n", int_to_bool(exp));
}

int main(int argc, char * argv[])
{
	char* bitmap = calloc(1, 5); 

    // mem_mngr_init();

    
	// // test your code here.


    // mem_mngr_leave();
    bitmap_print_bitmap(bitmap, 5);
    bitmap_set_bit(bitmap, 5, 32);
    bitmap_print_bitmap(bitmap, 5);

    free(bitmap);
    return 0;
}
