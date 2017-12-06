#include "stdafx.h"
#include "simeck32.h"
#include <stdint.h>

void simeck_32_64(uint16_t master_key[],const uint16_t plaintext[],uint16_t ciphertext[],int rounds)
{
    const int NUM_ROUNDS = rounds;
    int idx;

    uint16_t keys[4] = {master_key[0],master_key[1],master_key[2],master_key[3]};
    ciphertext[0] = plaintext[0];
    ciphertext[1] = plaintext[1];
    uint16_t temp;

    uint16_t constant = 0xFFFC;
    uint32_t sequence = 0x9A42BB1F;

    for (idx = 0; idx < NUM_ROUNDS; idx++)
	{
        ROUND32(keys[0],ciphertext[0],ciphertext[1],temp);
		
        constant &= 0xFFFC;
        constant |= sequence & 1;
        sequence >>= 1;

        ROUND32(constant,keys[1],keys[0],temp);

        // rotate the LFSR of keys
        temp = keys[1];
        keys[1] = keys[2];
        keys[2] = keys[3];
        keys[3] = temp;
    }
}

void simeck_32_64_const(uint16_t master_key[],uint16_t plaintext[],uint16_t ciphertext[],int rounds)
{
    const int NUM_ROUNDS = rounds;
    int idx;

    uint16_t keys[4] = {master_key[0],master_key[1],master_key[2],master_key[3],};
    ciphertext[0] = plaintext[0];
    ciphertext[1] = plaintext[1];
    uint16_t temp;

    uint16_t constant = 0xFFFC;
    uint32_t sequence = 0x9A42BB1F;

    for (idx = 0; idx < (NUM_ROUNDS -1); idx++)
	{
        ROUND32(keys[0],ciphertext[1],ciphertext[0],temp);

        constant &= 0xFFFC;
        constant |= sequence & 1;
        sequence >>= 1;

        ROUND32(constant,keys[1],keys[0],temp);

        // rotate the LFSR of keys
        temp = keys[1];
        keys[1] = keys[2];
        keys[2] = keys[3];
        keys[3] = temp;
    }

	// last round

	ROUND32_last(keys[0],ciphertext[1],ciphertext[0],temp);
}
