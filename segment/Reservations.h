//
// Created by spooky on 2016-04-03.
//

#ifndef PROJECT_RESERVATIONS_H
#define PROJECT_RESERVATIONS_H

#include "../shared/Maybe.h"
#include "../shared/entities.h"
#include "../shared/fs.h"
#include "ReservationSet.h"
#include "Segment.h"
#include <utility>

namespace db {
namespace fs {

template <typename T_Meta>
class Reservations {
private:
  Segment<T_Meta> m_segment;
  db::ReservationSet<T_Meta::lines()> m_reservations;

public:
  using T_Table = typename T_Meta::Table;

  explicit Reservations(Segment<T_Meta> &&seg)
      : m_segment{std::move(seg)}, m_reservations{m_segment.present_set()} {
  }

  Reservations(Reservations<T_Meta> &&o)
      : m_segment{std::move(o.m_segment)},
        m_reservations{std::move(o.m_reservations)} {
    //                db::assert_is_meta<T_Meta>();
  }

  Reservations(const Reservations &) = delete;

  sp::Maybe<Reservation<T_Table>> reserve() {
    auto optResId = m_reservations.reserve();
    if (optResId) {
      auto segment_id = m_segment.get_id();
      Reservation<T_Table> r{optResId.get(), segment_id};
      return sp::Maybe<Reservation<T_Table>>{r};
    }
    return sp::Maybe<Reservation<T_Table>>{};
  }

  void unreserve(const Reservation<T_Table> &) {
  }

  bool has_free() const;
};

template <typename T_Meta>
bool Reservations<T_Meta>::has_free() const {
  return m_reservations.has_free();
}
}
}

#endif // PROJECT_RESERVATIONS_H
