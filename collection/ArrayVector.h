#ifndef _COLLECTION_ARRAY_VECTOR_H
#define _COLLECTION_ARRAY_VECTOR_H
#include <array>
#include <stddef.h>
namespace sp {
template <typename T, size_t size>
class ArrayVector {
public:
  using value_type = T;
  // using typename std::vector<T>::difference_type difference_type;
  // using typename std::vector<T>::size_type size_type;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;

private:
  constexpr static size_t rounded_size() {
    return size;
  }
  std::array<value_type, rounded_size()> m_static;

public:
  ArrayVector() {
  }

  explicit ArrayVector(std::initializer_list<value_type> init) : ArrayVector() {
  }
};
}
#endif
