//
// Created by spooky on 2016-07-20.
//

#include "gtest/gtest.h"
#include "../LIFO.h"

TEST (LIFOTest, test) {

    LIFO<size_t> l;
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(1lu);
    ASSERT_EQ(1ul, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(20lu);
    ASSERT_EQ(20ul, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));

    l.push_front(1lu);
    l.push_front(20lu);
    size_t ssss(30);
    l.push_front(ssss);

    ASSERT_EQ(1lu, l.pop(0ul));
    ASSERT_EQ(20lu, l.pop(0ul));
    ASSERT_EQ(30lu, l.pop(0ul));
    ASSERT_EQ(0ul, l.pop(0ul));
}
