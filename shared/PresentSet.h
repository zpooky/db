#ifndef DB_PRESENTSET_H
#define DB_PRESENTSET_H

#include "PageRange.h"
#include "shared.h"
#include <bitset>

namespace db {
/**
 * key -> Index -> (segment::id, page::position)
 * segment::id -> Segment
 * page::position -> Segment -> value
 */
template <page::position capacity>
class PresentSet {
private:
  static_assert(capacity % 8 == 0, "");
  using Bitset_t = std::bitset<capacity>;

private:
  const Bitset_t m_bitset;
  const PageRange m_range;

public:
  PresentSet(const Bitset_t &init, page::position strt, page::position sz)
      : m_bitset(init), m_range(strt, sz) {
  }

  PresentSet(const PresentSet &) = delete;

  PresentSet(PresentSet &&o)
      : m_bitset(std::move(o.m_bitset)), m_range(o.m_range) {
  }

  ~PresentSet() {
  }

  bool operator<(const PresentSet<capacity> &o) const {
    return m_range < o.m_range;
  }

  bool operator[](page::position p) const {
    page::position idx(m_range.from_absolute(p));
    return m_range(idx);
  }

  const Bitset_t &get_bitset() const & {
    return m_bitset;
  }
  // Bitset_t &get_bitset() & {
  //   return m_bitset;
  // }
  // Bitset_t &&get_bitset() && {
  //   return std::move(m_bitset);
  // }
};
}
#endif // DB_PRESENTSET_H
