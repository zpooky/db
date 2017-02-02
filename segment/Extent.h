#ifndef _DB_SEGMENT_EXTENT_H
#define _DB_SEGMENT_EXTENT_H

#include "../segment/ReservationSet.h"
#include "../shared/PresentSet.h"

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
  using Table_t = typename Meta_t::latest;
  static constexpr size_t lines = Meta_t::extent_lines();
  using Reservation_t = db::ReservationSet<lines>;

private:
  Reservation_t m_reservations;
  const db::segment::id segment_id;

public:
  explicit Extent(db::segment::id sid) : m_reservations(), segment_id(sid) {
  }

  explicit Extent(db::segment::id sid, PresentSet<lines> &&p)
      : m_reservations{p.get_bitset()}, segment_id(sid) {
  }

  Extent(Extent &&o)
      : m_reservations{std::move(o.m_reservations)}, segment_id(o.segment_id) {
  }

  Extent(const Extent &) = delete;

public:
  sp::Maybe<Reservation<Table_t>> reserve() {
    auto optional_res = m_reservations.reserve();
    if (optional_res) {
      Reservation<Table_t> r{segment_id, optional_res.get()};
      return sp::Maybe<Reservation<Table_t>>{r};
    }
    return sp::Maybe<Reservation<Table_t>>{};
  }

  auto &reservations() {
    return m_reservations;
  }

  const auto &reservations() const {
    return m_reservations;
  }

  bool has_free() const {
    return m_reservations.has_free();
  }
};
}

#endif
