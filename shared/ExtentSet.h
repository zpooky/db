#ifndef DB_EXTENT_SET_H
#define DB_EXTENT_SET_H

#include "../collection/CachedAllocator.h"
#include "../shared/Configuration.h"
#include "PresentSet.h"
#include "shared.h"
#include <algorithm>

namespace db {
class HeapExtentSet {
public:
  using PS = db::PresentSet<db::Configuration::extent_lines()>;
private:
  using Extents_t = std::vector<PS>;

public:
  using Allocator = sp::CachedAllocator;

private:
  Extents_t m_extents;
  PageRange m_range;
  const db::segment::id m_segment;
  Allocator m_allocator;

public:
  HeapExtentSet(const db::segment::id &s, Allocator &&a, Extents_t &&exts)
      : m_extents{std::move(exts)}, m_range(0, 0), m_segment(s),
        m_allocator(std::move(a)) {
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

  bool operator[](page::position) const {
    auto begin(m_extents.begin());
    auto end(m_extents.end());
    // TODO
    // std::binary_search(begin, end, search);
    return false;
  }
};
}

#endif
