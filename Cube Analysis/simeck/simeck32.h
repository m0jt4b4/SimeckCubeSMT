#ifndef SIMECK32_H
#define SIMECK32_H

#include <stdint.h>

#define LROT16(x, r) (((x) << (r)) | ((x) >> (16 - (r))))

#define ROUND32(key, lft, rgt, tmp) do { \
    tmp = (lft); \
    lft = ((lft) & LROT16((lft), 5)) ^ LROT16((lft), 1) ^ (rgt) ^ (key); \
    rgt = (tmp); \
} while (0)

#define ROUND32_last(key, lft, rgt, tmp) do { \
    tmp = (lft); \
    lft = ((lft) & LROT16((lft), 5)) ^ LROT16((lft), 1) ^ (rgt); \
    rgt = (tmp); \
} while (0)


void simeck_32_64(uint16_t master_key[],const uint16_t plaintext[],uint16_t ciphertext[],int rounds);

void simeck_32_64_const(uint16_t master_key[],uint16_t plaintext[],uint16_t ciphertext[],int rounds);


#endif  // SIMECK32_H
