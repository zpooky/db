//
// Created by spooky on 2016-04-23.
//

#ifndef PROJECT_HASH_H
#define PROJECT_HASH_H

#include <array>
#include <string.h>

namespace hash {
    using std::array;


    uint16_t gen_crc16(const uint8_t *data, uint16_t size) {
        const int CRC16 = 0x8005;
        const int CRC32 = 0x04C11DB7;


        uint16_t out = 0;
        int bits_read = 0, bit_flag;

        /* Sanity check: */
        if (data == NULL)
            return 0;

        while (size > 0) {
            bit_flag = out >> 15;

            /* Get next bit: */
            out <<= 1;
            out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

            /* Increment bit counter: */
            bits_read++;
            if (bits_read > 7) {
                bits_read = 0;
                data++;
                size--;
            }

            /* Cycle check: */
            if (bit_flag)
                out ^= CRC16;

        }

        // item b) "push out" the last 16 bits
        int i;
        for (i = 0; i < 16; ++i) {
            bit_flag = out >> 15;
            out <<= 1;
            if (bit_flag)
                out ^= CRC16;
        }

        // item c) reverse the bits
        uint16_t crc = 0;
        i = 0x8000;
        int j = 0x0001;
        for (; i != 0; i >>= 1, j <<= 1) {
            if (i & out) crc |= j;
        }

        return crc;
    }

    template<size_t T_bytes>
    class crc {
    private:
    public:
        using result_type =array<uint8_t, T_bytes>;

        template<size_t t_size>
        result_type digest(const array<uint8_t, t_size> &buff) {
//            const uint8_t *c = "123456789";
//            gen_crc16(c, strlen(c));

            result_type res{0};

            return res;
        }


    };

    using crc32 = crc<4>;
}

#endif //PROJECT_HASH_H
