#ifndef DB_EXTENT_SET_H
#define DB_EXTENT_SET_H

#include "../collection/CachedAllocator.h"
#include "PresentSet.h"
#include "shared.h"

namespace db {
template <size_t size>
class ExtentSet {
private:
  using PS = db::PresentSet<size>;
  using Extents_t = std::vector<PS>;

public:
  using Alloc = sp::CachedAllocator<PS>;

private:
  Extents_t m_extents;
  PageRange m_range;
  const db::segment::id m_segment;
  Alloc &m_allocator;

public:
  ExtentSet(const db::segment::id &segment, const Alloc &alloc)
      : m_extents(), m_range(0, 0), m_segment(segment), m_allocator(alloc) {
  }

  /**
   * introduce new extents
   */
  void introduce(const db::PresentSet<size> &o) {
  }

  bool operator[](page::position p) const {
    return false;
  }
};
}

#endif
