#ifndef PROJECT_SEGMENTS_H
#define PROJECT_SEGMENTS_H

#include "../collection/List.h"
#include "../shared/Assertions.h"
#include "../shared/shared.h"
#include "../shared/vfs.h"
#include "Context.h"
#include "Reservations.h"
#include "Segment.h"
#include <algorithm>
#include <atomic>
#include <functional>
#include <vector>

namespace db {
/* Frontend for table segments
 * - used for aquiring a reservation
 *   behind the scenes creates new segments
 */
/**
 * Internal collection of segments
 * - contains and owns a list of existing segments
 * - will create new segments if there is no
 *   free lines available.
 */
template <typename Meta_t>
class Segments {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using page_t = typename Meta_t::Page;
  using Factory = typename Meta_t::PageFactory;
  using version_t = db::raw::version_t;
  using id_t = db::raw::id;

private:
  Factory m_factory;
  sp::List<Segment<Meta_t>> m_segments;
  std::atomic<db::segment::id> m_id;

public:
  explicit Segments(db::segment::id id, Factory &&factory,
                    std::vector<Segment<Meta_t>> &&p_segments)
      : m_factory{std::move(factory)}, m_segments{std::move(p_segments)},
        m_id(id) {
    //                db::assert_is_context<Meta_t>();
  }

  Segments(Segments<Meta_t> &&o)
      : m_factory{std::move(o.m_factory)}, m_segments{std::move(o.m_segments)},
        m_id(o.m_id.load()) {
  }

  ~Segments() {
  }

  Reservation<Table_t> reserve() {
    while (true) {
      auto &reservation = free_page();
      auto maybe_res = reservation.reserve();
      if (maybe_res.is_present()) {
        return maybe_res.get();
      }
    }
  }

  id_t create(const Reservation<Table_t> &t, const Table_t &data) {
    auto *segment = m_segments.search([&](const auto &seg) {
      auto eq = seg.id() == t.segment;
      return eq;
    });
    if (!segment) {
      /**
      * If segment is not found but we where able to reserve
      * something is very wrong
      */
      throw std::runtime_error("REALY unexpected exception");
    }
    auto &page = segment->page();
    return page.create(m_id++, t.position, data);
  }

private:
  Reservations<Meta_t> &free_page() {
    auto p = [](const Segment<Meta_t> &s) -> bool {
      auto &r = s.reservations();
      return r.has_free();
    };

    auto f = [&]() { return this->m_factory(); };
    auto &seg = m_segments.find(p, f);
    return seg.reservations();
  }
};
}
#endif // PROJECT_SEGMENTS_H
