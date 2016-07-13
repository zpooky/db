//
// Created by spooky on 2016-07-08.
//

#include "gtest/gtest.h"
#include "../CBitset.h"
#include <iostream>

using std::cout;
using std::endl;
using sp::CBitset;

class CBitsetTest : public ::testing::TestWithParam<const char *> {

};

INSTANTIATE_TEST_CASE_P(InstantiationName, CBitsetTest, ::testing::Values(
        "byte"
));

TEST_F(CBitsetTest, test_empty) {
    constexpr size_t bits = 1024;
    CBitset<bits> b;
    for (size_t i = 0; i < bits; ++i) {
        ASSERT_FALSE(b.test(i));
    }
}

template<size_t bits, typename T>
void true_set(CBitset<bits, T> &b) {
    for (size_t i = 0; i < bits; ++i) {
        for (size_t a = 0; a < i; ++a) {
            ASSERT_TRUE(b.test(a));
        }
        ASSERT_TRUE(b.set(i, true));
        for (size_t a = bits - 1; a > i; --a) {
            ASSERT_FALSE(b.test(a));
        }
    }
}

template<size_t bits, typename T>
void false_set(CBitset<bits, T> &b) {
    for (size_t i = 0; i < bits; ++i) {
        for (size_t a = 0; a < i; ++a) {
            ASSERT_FALSE(b.test(a));
        }
        ASSERT_TRUE(b.set(i, false));
        for (size_t a = bits - 1; a > i; --a) {
            ASSERT_TRUE(b.test(a));
        }
    }
}

template<typename T>
void test_seq_setFalse_get() {
    constexpr size_t bits = 1024;
    CBitset<bits, T> b;
    true_set(b);
    false_set(b);

}


TEST_F(CBitsetTest, test_seq_setFalse_get_short) {
    test_seq_setFalse_get<uint16_t>();
}

TEST_F(CBitsetTest, test_seq_setFalse_get_int) {
    test_seq_setFalse_get<uint32_t>();
}


TEST_F(CBitsetTest, test_seq_setFalse_get_byte) {
    test_seq_setFalse_get<uint8_t>();
}

TEST_F(CBitsetTest, test_seq_setFalse_get_long) {
    test_seq_setFalse_get<uint64_t>();
}

std::string random_binary(size_t cnt) {
    std::mt19937 mt(0);
    std::uniform_int_distribution<int> dist(0, 1);
    std::string stream = "";
    for (size_t i = 0; i < cnt; ++i) {
        if (dist(mt)) {
            stream.append("1");
        } else {
            stream.append("0");
        }
    }
    return stream;
}

struct Timer {
public:
    const std::clock_t start;

    Timer() : start{std::clock()} {
    }

    ~Timer() {
        std::clock_t stop = std::clock();
        cout << endl << (stop - start) << "ns" << endl;
    }
};

template<typename F>
auto time(F f) -> decltype(f()) {
    Timer t;
    return f();
}

template<typename T>
void test_init() {
    constexpr size_t bits(1024 * 80);
    std::string str = random_binary(bits);
    std::bitset<bits> init(str);
    using Bitset_t = CBitset<bits, T>;
//    cout << endl << init << endl;
    auto b = time([&]() -> Bitset_t {
        return Bitset_t(init);
    });
//    cout << b << endl;
    for (size_t i = 0; i < init.size(); ++i) {
        ASSERT_EQ(init.test(i), b.test(i));
    }
}

TEST_F(CBitsetTest, init_long) {
    test_init<uint64_t>();
}

TEST_F(CBitsetTest, init_set_fill) {
    constexpr size_t bits(1024 * 80);
    std::string str = random_binary(bits);
    std::bitset<bits> init(str);
    CBitset<bits, unsigned long> bb;
    time([&] {
        for (size_t i = 0; i < bits; ++i) {
            bb.set(i, init[i]);
        }
    });
}
//TEST_F(CBitsetTest, tesst) {
//    cout << endl << GetParam() << endl;
//}

TEST_F(CBitsetTest, test_true_random) {

}