/*
 * Program implements functions to:
 * 1. Convert 32b uints to sync-safe ints 
 * 2. Convert sync-safe ints to 32b uints 
 */

#include <stdint.h>

#include "syncint.h"

/*
 * Function converts 32bit integer to synchronized safe integer
 * MSB                            LSB
 * [31...24][23...16][15...8][7...0]
 *  1. right shift res by 8 to gradually move the data to the LSB
 *  2. copy 7 LSbs of number and left shift by 24 to bring it to the MSB
 *  3. or it with res to store it in the MSB
 *  4. right shift number by 7 to remove 7 LSbs
 *  5. goto 1
 */
uint32_t int_to_sync_safe_int(uint32_t number) {
    uint32_t res, copy;
    int count;

    // loop should run 4 times
    count = 3;              

    // intialize all bits in res to 0
    res = 0x0;

    // loop to convert integer to sync safe integer
    while(count-->=0) {
        res = res>>8;
        copy = (number&0x7f)<<24;
        res = res|copy;
        number = number>>7;
    }

    return res;
}

/*
 * Function converts sync-safe int to 32bit int 
 * MSB                            LSB
 * [31...24][23...16][15...8][7...0]
 *  1. left shift res by 7
 *  2. copy 7 bits number[30:24] 
 *  3. or copy with res and store result in res
 *  4. left shift number by 8
 *  5. goto 1
 */
int sync_safe_int_to_int(uint32_t number) {
    uint32_t res, copy;
    int count;

    // loop should run 4 times
    count = 3;              

    // intialize all bits in res to 0
    res = 0x0;

    // loop to convert sync-safe integer to 32b integer
    while(count-->=0) {
        res = res<<7;
        copy = (number&(0x7f<<24))>>24;
        res = res|copy;
        number = number<<8;
    }
    return res;
}

/* Driver Code
int main() {
    uint32_t digit;

    digit = 0x049ddfff;

    digit = int_to_sync_safe_int(digit);
    printf("0x%08x\n",digit);
    printf("0x%08x\n",sync_safe_int_to_int(digit));

    return 0;
}
*/
