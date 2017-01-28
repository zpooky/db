//
// Created by spooky on 2016-07-06.
//

#ifndef DB_PRESENTSET_H
#define DB_PRESENTSET_H

#include <bitset>

namespace db {
/**
 * key -> Index -> (segment::id, page::position)
 * segment::id -> Segment
 * page::position -> Segment -> value
 */
template <size_t size>
class PresentSet {
private:
  static_assert(size % 8 == 0, "");
  using Bitset_t = std::bitset<size>;
  const Bitset_t m_bitset;

public:
  PresentSet() : m_bitset{} {
  }

  explicit PresentSet(const Bitset_t &init) : m_bitset(init) {
  }

  PresentSet(const PresentSet &) = delete;

  PresentSet(PresentSet &&o) : m_bitset(std::move(o.m_bitset)) {
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
