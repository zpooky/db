#ifndef DB_PRESENT_SET_H
#define DB_PRESENT_SET_H

#include "../collection/HeapBitset.h"
#include "PageRange.h"
#include "shared.h"
#include <bitset>
#include <cassert>

namespace db {
/**
 * key -> Index -> (segment::id, page::position)
 * segment::id -> Segment
 * page::position -> Segment -> value
 */

template <size_t capacity>
class PresentSet {
private:
  static_assert(capacity % 8 == 0, "");
  using Bitset_t = std::bitset<capacity>;

private:
  Bitset_t m_bitset;
  const PageRange m_range;

public:
  /**
   * strt: absolute
   * sz: relative to strt
   *
   * present range[strt, (strt+sz)]
   */
  PresentSet(const Bitset_t &init, const PageRange &range)
      : m_bitset(init), m_range(range) {
    assert(range.size() <= capacity);
  }

  PresentSet(const PresentSet &) = delete;

  PresentSet(PresentSet &&o)
      : m_bitset(std::move(o.m_bitset)), m_range(o.m_range) {
  }

  ~PresentSet() {
  }

  bool operator<(page::position p) const {
    return m_range < p;
  }

  bool operator<(const PresentSet<capacity> &o) const {
    return m_range < o.m_range;
  }

  bool operator==(page::position p) const {
    return m_range.in_range(p);
  }

  bool operator[](page::position p) const {
    auto idx = m_range.from_absolute(p);
    return m_bitset[idx];
  }

  const PageRange &range() const & {
    return m_range;
  }

  const Bitset_t &bitset() const & {
    return m_bitset;
  }

  Bitset_t &bitset() & {
    return m_bitset;
  }

  Bitset_t &&bitset() && {
    return std::move(m_bitset);
  }
};

class HeapPresentSet {
private:
  sp::HeapBitset m_bitset;
  const PageRange m_range;

public:
  template <size_t capacity>
  HeapPresentSet(const sp::CachedAllocator &a, const PresentSet<capacity> &init,
                 const PageRange &r)
      : m_bitset(init.bitset(), a), m_range(r) {
    assert(m_range.size() <= capacity);
  }

  HeapPresentSet(const HeapPresentSet &) = delete;

  HeapPresentSet(HeapPresentSet &&o)
      : m_bitset(std::move(o.m_bitset)), m_range(o.m_range) {
  }

  ~HeapPresentSet() {
  }

  bool operator<(page::position p) const {
    return m_range < p;
  }
  bool operator<(const HeapPresentSet &o) const {
    return m_range < o.m_range;
  }
};
}
#endif // DB_PRESENTSET_H
