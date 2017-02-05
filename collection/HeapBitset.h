#ifndef _SP_COLLECTION_HEAP_BITSET_H
#define _SP_COLLECTION_HEAP_BITSET_H

#include "CachedAllocator.h"
#include <cstddef>
#include <bitset>

namespace sp {

class HeapBitset {
private:
  CachedAllocator &m_allocator;
  const size_t m_size;

public:
  template <size_t size>
  HeapBitset(const std::bitset<size> &init, CachedAllocator &a)
      : m_allocator(a), m_size(init.size()) {
  }

  ~HeapBitset() {
    // m_allocator.deallocate();
  }
};
}

#endif
