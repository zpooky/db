#include "gtest/gtest.h"
#include "../HashMap.h"



class HashMapTest : public ::testing::TestWithParam<size_t> {

};

INSTANTIATE_TEST_CASE_P(Threads, HashMapTest, ::testing::Range(size_t(1), size_t(15)));

using namespace sp;

// TEST_F (HashMapTest, test) {
// }
//

TEST(HashMapTest, test) {
}
