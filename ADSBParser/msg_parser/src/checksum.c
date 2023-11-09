#include "defs.h"
#include "checksum_table.h"

uint32_t computeChecksum(unsigned char *msg, int bits) {
    uint32_t crc = 0;
    int offset = (bits == 112) ? 0 : (112-56);
    int j;

    for(j = 0; j < bits; j++) {
        int byte = j/8;
        int bit = j%8;
        int bitmask = 1 << (7-bit);

        /* If bit is set, xor with corresponding table entry. */
        if (msg[byte] & bitmask)
            crc ^= modes_checksum_table[j+offset];
    }
    return crc; /* 24 bit checksum. */
}