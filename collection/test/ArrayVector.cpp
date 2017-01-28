#include "../ArrayVector.h"
#include "gtest/gtest.h"

class ArrayVectorTest : public ::testing::TestWithParam<size_t> {};

INSTANTIATE_TEST_CASE_P(Threads, ArrayVectorTest,
                        ::testing::Range(size_t(1), size_t(15)));

using namespace sp;

// TEST_F(ArrayVectorTest, test) {
// }

template <typename T, size_t size>
constexpr size_t rounded_size() {
  return (size * sizeof(T)) % 8 == 0 ? size : 0;
}

TEST(ArrayVectorTest, test) {
  

}
