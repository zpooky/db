#include "../Binary.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

// #include "gtest/gtest.h"
//
using namespace sp::binary;
using namespace std;
//
// TEST(BinaryTest, test) {
//   using Type = int;
//   std::array<Type, 1024> buff{};
//   size_t sz(0);
//
//   std::array<Type, buff.size()> range{};
//   size_t it(0);
//   std::for_each(range.begin(), range.end(), [&](auto &b) {
//     //
//     b = it++;
//   });
//   std::random_shuffle(range.begin(), range.end());
//
//   size_t size(0);
//   // for (auto &c : range)
//   {
//     size_t c(0);
//     insert(buff, sz, Type(c));
//     ASSERT_TRUE(find(buff, sz, Type(c)));
//     ASSERT_TRUE(std::binary_search(buff.begin(), buff.end(), c));
//     ASSERT_EQ(++size, sz);
//   }
// }

int main(int, char **) {
  using Type = int;
  constexpr size_t size(55);
  std::array<Type, size> buff{};
  size_t sz(0);

  //
  std::array<Type, buff.size()> range{};
  size_t it(size);
  std::for_each(range.begin(), range.end(), [&](auto &b) {
    //
    b = --it;
  });
  // std::random_shuffle(range.begin(), range.end());
  //

  for (size_t i(0); i < size; ++i) {
    insert(buff, sz, Type(range[i]));
  }
  for (size_t i(0); i < sz; ++i) {
    cout << buff[i] << ",";
  }
  cout << endl;
}
