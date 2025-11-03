/*
 * CS 551 Project "Memory manager".
 * This file needs to be turned in.	
 */

#include "common.h"

/*
 * Test if the value of the "pos"-th bit (starting from 0) in the "bitmap" is 1.
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bit map
 * @param pos: the position of the bit of interest
 * @return: the value of the bit of interest (i.e., 0 or 1) on success
 *          BITMAP_OP_ERROR (defined in "common.h") on any errors
 */
int bitmap_bit_is_set(unsigned char* bitmap, int size, int target_pos)
{
    // make sure paramters are valid
    if (target_pos >= (size << 3)) {
        return BITMAP_OP_ERROR;
    } else {
        // get the byte index and postion within the byte of the target_pos
        int idx = target_pos >> 3;
        int byte_pos = target_pos & 7;

        // and the mask to determine if the bit of interest is set or not
        return (1 << byte_pos) & bitmap[idx];
    }
}

/*
 * Finds the position of the first set bit in the bitmap
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bitmap
 * @return: the position of the first set bit in the bitmap on success
            BITMAP_OP_NOT_FOUND on any errors.
*/
static int find_first_set_bit(unsigned char* bitmap, int size) {
    for (int i = 0; i < size; i++) {
        if (bitmap_bit_is_set(bitmap, size, i)) {
            return i;
        }
    }
    return BITMAP_OP_NOT_FOUND;
}

/*
 * Finds the position of the first set bit in the bitmap
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bitmap
 * @return: the position of the first set bit in the bitmap on success
            BITMAP_OP_NOT_FOUND on any errors.
*/
static int find_first_unset_bit(unsigned char* bitmap, int size) {
    for (int i = 0; i < size; i++) {
        if (!bitmap_bit_is_set(bitmap, size, i)) {
            return i;
        }
    }
    return BITMAP_OP_NOT_FOUND;
}

/*
 * Find the position (starting from 0) of the first bit that has value "val" in the "bitmap". 
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bitmap
 * @param val: 0 or non-0
 * @return: the position in the bitmap of the first 0 bit (if "val" is zero) or the first 1 bit (if "val" is non-zero)
 *          BITMAP_OP_NOT_FOUND (defined in "common.h") if the bit of interest does not exist in the bitmap
 *          BITMAP_OP_ERROR (defined in "common.h") on any other errors
 */
int bitmap_find_first_bit(unsigned char* bitmap, int size, int val) {
    if (val) {
        return find_first_set_bit(bitmap, size);
    } else {
        return find_first_unset_bit(bitmap, size);
    }
}

/*
 * Change the value of the "target_pos"-th bit (starting from 0) in the "bitmap" to 1.
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bit map
 * @param target_pos: the position of the bit of interest
 * @return: BITMAP_OP_SUCCEED (defined in "common.h") on success
 *          BITMAP_OP_ERROR (defined in "common.h") on any errors
 */
int bitmap_set_bit(unsigned char* bitmap, int size, int target_pos)
{
    // make sure the parameters are valid
    if (target_pos >= (size << 3)) {
        return BITMAP_OP_ERROR;
    } else {
        // get the byte index and position within the byte of the target pos
        int idx = target_pos >> 3;
        int byte_pos = target_pos & 7;

        // or it with the mask so other bits aren't affected
        bitmap[idx] |= 1 << byte_pos;
        return BITMAP_OP_SUCCEED;
    }
}

/*
 * Change the value of the "target_pos"-th bit (starting from 0) in the "bitmap" to 0.
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bit map
 * @param target_pos: the position of the bit of interest
 * @return: BITMAP_OP_SUCCEED (defined in "common.h") on success
 *          BITMAP_OP_ERROR (defined in "common.h") on any errors
 */
int bitmap_clear_bit(unsigned char* bitmap, int size, int target_pos)
{
    if (target_pos >= (size << 3)) {
        return BITMAP_OP_ERROR;
    } else {
        // get the byte index and position within the byte of the target pos
        int idx = target_pos >> 3;
        int byte_pos = target_pos & 7;

        // mask where every bit is set except in the byte_pos position
        char mask = ~(1 << byte_pos);

        // and with the mask to clear the byte_pos bit
        bitmap[idx] &= mask;
        return BITMAP_OP_SUCCEED;
    }
}

/*
 * Print the content of a bitmap in starting from the first bit, and insert a space every 4 bits
 * @param bitmap: start address of the bitmap
 * @param size: size in bytes of the bit map
 * @return: BITMAP_OP_SUCCEED (defined in "common.h") on success
 *          BITMAP_OP_ERROR (defined in "common.h") on any errors
 * Do not change the implementation of this function.
 */
int bitmap_print_bitmap(unsigned char* bitmap, int size)
{
    int pos = 0;
    int total_bits = size * BIT_PER_BYTE;
    unsigned char current_byte = 0;

    if (NULL == bitmap)
    {
        printf("ERROR: NULL bit map!\n");
        return BITMAP_OP_ERROR;
    }

    printf("bitmap %p size %d bytes: ", bitmap, size);

    while (pos < total_bits)
    {
        int v = 0;
        
        current_byte = *(bitmap + pos/BIT_PER_BYTE);
        v = ((current_byte >> pos % BIT_PER_BYTE) & 0x01);
        printf("%d", v);
        if ((pos + 1) % 4 == 0)
        {
            printf("  ");
        }
        
        pos++;
    }

    printf("\n");

    return BITMAP_OP_SUCCEED;
}
