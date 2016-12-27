//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../fs/FileWriter.h"
#include "../journal/Journals.h"
#include "../shared/Assertions.h"
#include "../shared/entities.h"
#include "PresentSet.h"
#include "Reservations.h"

namespace db {
namespace fs {
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
  PresentSet<Meta_t> m_lines;
  Reservations<Meta_t> m_reservations;

public:
  explicit Extent(db::segment::id sid, PresentSet<Meta_t> &&p)
      : m_lines{std::move(p)}, m_reservations{sid, m_lines} {
  }
  Extent(Extent &&o)
      : m_lines{std::move(o.m_lines)},
        m_reservations{std::move(o.m_reservations)} {
  }
  Extent(const Extent &) = delete;

public:
  const PresentSet<Meta_t> &present_set() const {
    return m_lines;
  }
  const Reservations<Meta_t> &reservations() const {
    return m_reservations;
  }
  Reservations<Meta_t> &reservations() {
    return m_reservations;
  }
};

template <typename Meta_t>
class Segment {
private:
  using hash_algh = typename Meta_t::hash_algh;
  using Table_t = typename Meta_t::Table;
  using Page_t = typename Meta_t::Page;
  using version_t = db::raw::version_t;

private:
  Extent<Meta_t> m_extents;
  Page_t m_page;
  // State
public:
  //                db::assert_is_meta<Meta_t>();
  explicit Segment(Page_t &&page, PresentSet<Meta_t> &&lines)
      : m_extents{page.id(), std::move(lines)}, m_page{std::move(page)} {
  }

  Segment(Segment<Meta_t> &&o)
      : m_extents{std::move(o.m_extents)}, m_page{std::move(o.m_page)} {
  }

  Segment(const Segment &) = delete;

  const Reservations<Meta_t> &reservations() const {
    return m_extents.reservations();
  }
  Reservations<Meta_t> &reservations() {
    return m_extents.reservations();
  }

  auto id() const {
    return m_page.id();
  }
  Page_t &page() {
    return m_page;
  }
};
}
}

#endif // FS_SEGMENT_H
