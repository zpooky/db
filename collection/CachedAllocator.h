#ifndef DB_COLLECTION_CACHED_ALLOCATOR_H
#define DB_COLLECTION_CACHED_ALLOCATOR_H
#include <cstddef>
namespace sp {
template <typename T>
class CachedAllocator {
private:
  const size_t m_cached;

public:
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const pointer;
  using const_void_pointer = const void *;
  using size_type = size_t;

public:
  explicit CachedAllocator(size_t cached) : m_cached(cached) {
  }

  ~CachedAllocator() {
  }

  bool operator=(const CachedAllocator<T> &o) const noexcept {
    return this == &o;
  }

  bool operator!=(const CachedAllocator<T> &o) const noexcept {
    return !operator=(o);
  }

  T *allocate(size_t);

  void deallocate(T *, size_t);
};

template <typename T>
T *CachedAllocator<T>::allocate(size_type) {
  return nullptr;
}

template <typename T>
void CachedAllocator<T>::deallocate(T *, size_type) {
}
}
#endif
