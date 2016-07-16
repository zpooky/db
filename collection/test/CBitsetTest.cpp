//
// Created by spooky on 2016-07-08.
//

#include "gtest/gtest.h"
#include "../CBitset.h"
#include <iostream>
#include <unordered_set>

using std::cout;
using std::endl;
using sp::CBitset;

class CBitsetTest : public ::testing::TestWithParam<bool> {

};

INSTANTIATE_TEST_CASE_P(PreFill, CBitsetTest, ::testing::Values(
        true, false
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
    const std::string m_msg;
    const std::clock_t start;

    Timer(const std::string &msg) :
            start{std::clock()},
            m_msg{msg} {
    }

    ~Timer() {
        std::clock_t stop = std::clock();
        cout << endl << m_msg << ": " << (stop - start) << "ns" << endl;
    }
};

template<typename F>
auto time(const std::string &msg, F f) -> decltype(f()) {
    Timer t(msg);
    return f();
}

template<typename T>
void test_init() {
    constexpr size_t bits(1024 * 80);
    std::string str = random_binary(bits);
    std::bitset<bits> init(str);
    using Bitset_t = CBitset<bits, T>;
//    cout << endl << init << endl;

    auto b = time(__PRETTY_FUNCTION__, [&]() -> Bitset_t {
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

TEST_F(CBitsetTest, init_int) {
    test_init<uint32_t>();
}

TEST_F(CBitsetTest, init_short) {
    test_init<uint16_t>();
}

TEST_F(CBitsetTest, init_byte) {
    test_init<uint8_t>();
}

TEST_F(CBitsetTest, init_set_fill) {
    constexpr size_t bits(1024 * 80);
    std::string str = random_binary(bits);
    std::bitset<bits> init(str);
    CBitset<bits, unsigned long> bb;
    time("init_set<unsigned long>", [&] {
        for (size_t i = 0; i < bits; ++i) {
            bb.set(i, init[i]);
        }
    });
}


template<typename T>
void test_set_random(bool v) {
    constexpr size_t bits(1024);
    CBitset<bits, T> bb(!v);
    std::array<size_t, bits> in;
    {
        size_t val = 0;
        std::generate(in.begin(), in.end(), [&] {
            return val++;
        });
    }
    std::shuffle(in.begin(), in.end(), std::default_random_engine(0));
    std::unordered_set<size_t> present;
    for (const size_t &it : in) {
        ASSERT_TRUE(bb.set(it, v));
        ASSERT_TRUE(present.insert(it).second);
        for (size_t i = 0; i < bb.size(); ++i) {
            if (present.find(i) != present.end()) {
                if (v) {
                    ASSERT_TRUE(bb.test(i));
                } else {
                    ASSERT_FALSE(bb.test(i));
                }
            } else {
                if (v) {
                    ASSERT_FALSE(bb.test(i));
                } else {
                    ASSERT_TRUE(bb.test(i));
                }
            }
        }
    }

}

TEST_P(CBitsetTest, test_long_random) {
    test_set_random<uint64_t>(GetParam());
}

TEST_P(CBitsetTest, test_int_random) {
    test_set_random<uint32_t>(GetParam());
}

TEST_P(CBitsetTest, test_short_random) {
    test_set_random<uint16_t>(GetParam());
}

TEST_P(CBitsetTest, test_byte_random) {
    test_set_random<uint8_t>(GetParam());
}

template<typename T>
void test_find(bool v) {
    constexpr size_t bits(1024);
    CBitset<bits, T> bb{!v};
    for (size_t i = 0; i < bits; ++i) {
//        cout << "(" << v << ")" << i << endl;
        ASSERT_EQ(bb.size() - 1, bb.find_first(i, v));
        ASSERT_TRUE(bb.set(i, v));
        ASSERT_EQ(v, bb.test(i));
        ASSERT_EQ(i, bb.find_first(i, v));
    }
}

TEST_P(CBitsetTest, test_findlong) {
    test_find<uint64_t>(GetParam());
}

TEST_P(CBitsetTest, test_findint) {
    test_find<uint32_t>(GetParam());
}

TEST_P(CBitsetTest, test_findshort) {
    test_find<uint16_t>(GetParam());
}

TEST_P(CBitsetTest, test_findbyte) {
    test_find<uint8_t>(GetParam());
}


template<typename T>
void test_find_reverse(bool v) {
    constexpr size_t bits(1024);
    CBitset<bits, T> bb{!v};
    for (size_t i = bb.size() - 1; i >= 0; --i) {
        ASSERT_TRUE(bb.set(i, v));
        ASSERT_EQ(v, bb.test(i));
        ASSERT_EQ(i, bb.find_first(v));
    }
}

TEST_P(CBitsetTest, test_findlong_reverse) {
    test_find<uint64_t>(GetParam());
}

TEST_P(CBitsetTest, test_findint_reverse) {
    test_find<uint32_t>(GetParam());
}

TEST_P(CBitsetTest, test_findshort_reverse) {
    test_find<uint16_t>(GetParam());
}

TEST_P(CBitsetTest, test_findbyte_reverse) {
    test_find<uint8_t>(GetParam());
}
