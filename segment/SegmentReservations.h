//
// Created by spooky on 2016-03-06.
//

#ifndef FS_RESERVATION_H
#define FS_RESERVATION_H

#include "../shared/fs.h"
#include "Segment.h"

namespace db {
namespace fs {
/**
 * # Segment
 * |======.---------.
 * |Page  |   Line   |<---| Reserve
 * |      |----------|    |
 * |      |   Line   |<---|
 * |======|----------|    |
 * |Page  |   Line   |    |
 * |      |----------|    |
 * |      |   Line   |    |
 * |======|----------|    |
 *
 *
 * Frontend for single segment reservations.
 * Thread safe
 */
template <typename t_Table>
class SegmentReservations {
private:
  const Segment<t_Table> m_segment;

public:
  explicit SegmentReservations(const Segment<t_Table> &s) : m_segment(s) {
    db::assert_is_table<t_Table>();
  }

  /* Reserve a single line.
   * May fail with returning a not valid reservation.
   * Reservation validty can be checked with Reservation.is_valid()
   */
  Reservation<t_Table> reserve();

  /* Number of unreserved lines
   */
  size_t free_count() const {
    return size_t(0);
  }
};

template <typename t_Table>
Reservation<t_Table> SegmentReservations<t_Table>::reserve() {
  return Reservation<t_Table>{};
}
}
}

#endif // PROJECT_RESERVATION_H
