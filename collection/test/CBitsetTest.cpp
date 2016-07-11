//
// Created by spooky on 2016-07-08.
//

#include "gtest/gtest.h"
#include "../CBitset.h"
#include <iostream>

using std::cout;
using std::endl;
using sp::CBitset;
using std::atomic;
using Byte_t = uint8_t;
using Entry_t =atomic<Byte_t>;
static constexpr size_t byte_size = sizeof(Byte_t) * 8;

size_t byte_index(size_t idx) {
    return std::ceil((double) idx / byte_size);
}


using ttttt  =unsigned long long;

TEST(CBitset, test) {
    size_t bitIdx = 19;
//    size_t byteIdx = byte_index(bitIdx);

    const Byte_t offset = bitIdx - (ttttt(bitIdx / byte_size) * 8);

    constexpr Byte_t one_ = 1 << (byte_size - 1);//10000000
    const Byte_t mask = one_ >> offset;

    Byte_t bbb = Byte_t(128) | mask;
    std::bitset<byte_size> b{mask};
    cout << endl;
    cout << b << endl;
}



TEST(CBitset, test_empty) {
    constexpr size_t bits = 1024;
    CBitset<bits> b;
    for (size_t i = 0; i < bits; ++i) {
        ASSERT_FALSE(b.test(i));
    }
}

TEST(CBitset, test_seq_setTrue_get) {
    constexpr size_t bits = 1024;
    CBitset<bits> b;
    for (size_t i = 0; i < bits; ++i) {
        for (size_t a = 0; a < i; ++a) {
            ASSERT_TRUE(b.test(a));
        }
        ASSERT_TRUE(b.set(i, true));
    }
}