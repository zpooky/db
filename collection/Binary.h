#ifndef SP_COLLECTION_BINARY_H
#define SP_COLLECTION_BINARY_H

#include <array>

namespace sp {
namespace binary {

static size_t middle(size_t start, size_t stop) {
  return start + ((stop - start) / 2);
}

template <typename T>
static void swap(T *arr, size_t sz, T &&d) {
  for (size_t i(0); i <= sz; ++i) {
    std::swap(*(arr)++, d);
  }
}

template <typename T, size_t capacity>
bool find(std::array<T, capacity> &c, size_t &sz, const T &d) {
  return true;
}

template <typename T, size_t capacity>
void insert(std::array<T, capacity> &c, size_t &sz, T &&d) {
  size_t begin(0);
  size_t end(sz);
  std::less<T> ls;
start:
  size_t mid(middle(begin, end));
  if (mid == begin) {
    swap(c.data() + end, sz, std::move(d));
    ++sz;
    return;
  } else if (ls(c[mid], d)) {
    end = mid;
  } else {
    begin = mid;
  }
  goto start;
}
} // namespace binary
} // namespace sp


#endif
