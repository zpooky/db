#ifndef DB_COLLECTION_CACHED_ALLOCATOR_H
#define DB_COLLECTION_CACHED_ALLOCATOR_H

#include <cstddef>

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
  explicit CachedAllocator(size_t cached) : m_cached(cached) {
  }

  CachedAllocator(CachedAllocator &&o) : m_cached(o.m_cached) {
  }

  CachedAllocator(const CachedAllocator &) = delete;

  ~CachedAllocator() {
  }

  template <typename T>
  T *allocate(size_t);

  template <typename T>
  void deallocate(T *, size_t);
};

template <typename T>
T *CachedAllocator::allocate(size_t) {
  return nullptr;
}

template <typename T>
void CachedAllocator::deallocate(T *, size_t) {
}
} // namespace sp
#endif
