#ifndef DB_SHARED_PAGE_RANGE_H
#define DB_SHARED_PAGE_RANGE_H

#include "shared.h"
#include <cassert>

namespace db {

struct PageRange {
private:
  // absolute position
  page::position m_start;
  // relative to start
  size_t m_size;

public:
  PageRange(page::position p_start, size_t p_size)
      : m_start(p_start), m_size(p_size) {
  }

  bool operator<(const PageRange &o) const {
    return begin() < o.begin();
  }
  bool operator=(page::position p) const {
    return in_range(p);
  }
  bool operator<(page::position p) const {
    return begin() < p;
  }
  bool operator>(page::position p) const {
    // since end is exclusive and p is absolute >= is used
    return end() >= p;
  }

  bool in_range(page::position p) const {
    return p >= begin() && p < end();
  }

  page::position from_absolute(page::position p) const {
    assert(in_range(p));
    return page::position(p - m_start);
  }

  /*
   * Absolute begin
   */
  page::position begin() const {
    return m_start;
  }

  /*
   * Absolute end
   */
  page::position end() const {
    return page::position(m_start + m_size);
  }

  /**
   * Number of positions
   */
  size_t size() const {
    return m_size;
  }
};
}
#endif
