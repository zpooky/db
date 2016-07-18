//
// Created by spooky on 2016-07-17.
//

#include "gtest/gtest.h"
#include "../Buffer.h"

using std::cout;
using std::endl;
using db::Buffer;

TEST(BufferTest, test) {
    cout<<endl;
    constexpr size_t size = 1024;
    Buffer<size> b;
//    b.put(1ul);
    uint64_t set = 0xB38F0F83F03F80FF;
    db::le::put(b, set);
    ASSERT_EQ(size_t(8), b.postion());
    ASSERT_EQ(b.size(), b.postion());
    ASSERT_EQ(b.capacity(), size);

    b.flip();
    ASSERT_EQ(size_t(0), b.postion());
    ASSERT_EQ(b.size(), size_t(8));

    uint64_t read = db::le::get_uint64(b);
    ASSERT_EQ(set, read);
    ASSERT_EQ(b.size(), b.postion());
}
