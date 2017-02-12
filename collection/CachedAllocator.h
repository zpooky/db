#ifndef DB_COLLECTION_CACHED_ALLOCATOR_H
#define DB_COLLECTION_CACHED_ALLOCATOR_H

#include <cstddef>
#include <stdlib.h>

namespace sp {

class CachedAllocator {
private:
  const size_t m_cached;

public:
  // using value_type = T;
  // using pointer = value_type *;
  // using const_pointer = const pointer;
  // using const_void_pointer = const void *;
  // using size_type = size_t;

public:
  CachedAllocator(void *, size_t cached) : m_cached(cached) {
  }

  CachedAllocator(CachedAllocator &&o) : m_cached(o.m_cached) {
  }

  CachedAllocator(const CachedAllocator &) = delete;

  ~CachedAllocator() {
  }

  template <typename T>
  T *allocate();

  template <typename T>
  void deallocate(T *);
};

template <typename T>
T *CachedAllocator::allocate() {
  return (T *)malloc(sizeof(T));
}

template <typename T>
void CachedAllocator::deallocate(T *d) {
  free(d);
}

class ParrentAllocator {
private:
public:
  /**
   * single threaded using std::bitset
   */
  template <typename T>
  CachedAllocator child_st(size_t) {
    return CachedAllocator(nullptr, 0);
  }
};
} // namespace sp
#endif
