#ifndef DB_EXTENT_SET_H
#define DB_EXTENT_SET_H

#include "../collection/CachedAllocator.h"
#include "PresentSet.h"
#include "shared.h"
#include <algorithm>

namespace db {
class HeapExtentSet {
private:
  using PS = db::HeapPresentSet;
  using Extents_t = std::vector<PS>;

public:
  using Allocator = sp::CachedAllocator;

private:
  Extents_t m_extents;
  PageRange m_range;
  const db::segment::id m_segment;
  Allocator &m_allocator;

public:
  HeapExtentSet(const db::segment::id &segment, Allocator &alloc)
      : m_extents{}, m_range(0, 0), m_segment(segment), m_allocator(alloc) {
  }

  /**
   * introduce new extents
   */
  template <size_t capacity>
  void introduce(const db::PresentSet<capacity> &o) {
    // TODO sort
    m_extents.emplace_back(o, m_allocator);
    auto size(m_range.size() + o.range().size());
    assert(size > m_range.size());
    m_range = PageRange(0, size);
  }

  bool operator[](page::position search) const {
    auto begin(m_extents.begin());
    auto end(m_extents.end());
    // std::binary_search(begin, end, search);
    return false;
  }
};
}

#endif
