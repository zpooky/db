//
// Created by spooky on 2016-04-03.
//

#ifndef PROJECT_RESERVATIONS_H
#define PROJECT_RESERVATIONS_H

#include "../shared/Maybe.h"
#include "../shared/entities.h"
#include "../shared/shared.h"
#include "ReservationSet.h"
#include "Segment.h"
#include <utility>

namespace db {
namespace fs {

template <typename Meta_t>
class Reservations {
private:
  using Table_t = typename Meta_t::Table;

private:
  const db::segment::id segment_id;
  db::ReservationSet<Meta_t::extent_lines()> m_reservations;

public:
  //                db::assert_is_meta<Meta_t>();
  explicit Reservations(db::segment::id id, const PresentSet<Meta_t> &p)
      : segment_id{id}, m_reservations{p} {
  }

  Reservations(Reservations<Meta_t> &&o)
      : segment_id{o.segment_id}, m_reservations{std::move(o.m_reservations)} {
  }

  Reservations(const Reservations &) = delete;

  sp::Maybe<Reservation<Table_t>> reserve() {
    auto optional_res = m_reservations.reserve();
    if (optional_res) {
      Reservation<Table_t> r{segment_id, optional_res.get()};
      return sp::Maybe<Reservation<Table_t>>{r};
    }
    return sp::Maybe<Reservation<Table_t>>{};
  }

  bool has_free() const;
};

template <typename Meta_t>
bool Reservations<Meta_t>::has_free() const {
  return m_reservations.has_free();
}
}
}

#endif // PROJECT_RESERVATIONS_H
