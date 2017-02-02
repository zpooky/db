#ifndef DB_SHARED_PAGE_RANGE_H
#define DB_SHARED_PAGE_RANGE_H

#include "shared.h"
#include <cassert>

namespace db {
struct PageRange {
  const page::position m_start;
  const page::position m_capacity;

  PageRange(page::position p_start, page::position p_capacity)
      : m_start(p_start), m_capacity(p_capacity) {
  }

  bool operator<(const PageRange &o) const {
    return m_start < o.m_start;
  }

  bool in_range(page::position p) const {
    page::position max(m_start + m_capacity);
    return p >= m_start && p < max;
  }

  page::position from_absolute(page::position p) const {
    assert(in_range(p));
    return page::position(p - m_start);
  }
};
}
#endif
