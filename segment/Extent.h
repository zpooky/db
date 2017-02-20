#ifndef _DB_SEGMENT_EXTENT_H
#define _DB_SEGMENT_EXTENT_H

#include "../shared/PresentSet.h"
#include "../collection/Bitset.h"
#include "../shared/Maybe.h"
#include <atomic>

namespace db {
/**
 * Represent a fix arena of data
 * Segment
 * |Extent 0
 * |Extent 1
 * |...
 */
template <typename Meta_t>
class Extent {
private:
  static constexpr size_t lines = Meta_t::extent_lines();

private:
  sp::con::Bitset<lines, uint64_t> m_reservations;
  /**
   * cache
   */
  std::atomic<size_t> m_cnt;

public:
  explicit Extent(PresentSet<lines> &&p)
      : m_reservations{p.bitset()}, m_cnt(m_reservations.find_first(false)) {
  }

  Extent(Extent &&o)
      : m_reservations{std::move(o.m_reservations)}, m_cnt{o.m_cnt.load()} {
  }

  Extent(const Extent &) = delete;

  ~Extent(){
  }

public:
  sp::Maybe<page::position> reserve() {
    auto start = m_cnt.load();
    if (start != m_reservations.npos) {
      size_t reserved = m_reservations.swap_first(start, true);
      if (reserved != m_reservations.npos) {
        m_cnt.store(reserved);
        return sp::Maybe<page::position>{reserved};
      }
    }
    return sp::Maybe<page::position>{};
  }

  bool has_free() const {
    auto start = m_cnt.load();
    if (start != m_reservations.npos) {
      bool result = !m_reservations.all(start, true);
      return result;
    }
    return false;
  }
};

} // namespace db

#endif
