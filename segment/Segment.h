#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "Extents.h"

namespace db {

template <typename Meta_t>
class Segment {
private:
  using hash_algh = typename Meta_t::hash_algh;
  using Table_t = typename Meta_t::Table;
  using Page_t = typename Meta_t::Page;
  using version_t = db::raw::version_t;
  using Extents_t = Extents<Meta_t>;

private:
  Extents_t m_extents;
  Page_t m_page;
  // State
public:
  //                db::assert_is_meta<Meta_t>();
  explicit Segment(Page_t &&page, Extents_t &&extents)
      : m_extents{std::move(extents)}, m_page{std::move(page)} {
  }

  Segment(Segment<Meta_t> &&o)
      : m_extents{std::move(o.m_extents)}, m_page{std::move(o.m_page)} {
  }

  Segment(const Segment &) = delete;

  Segment& operator=(const Segment&) = delete;
  Segment& operator=(const Segment&&) = delete;

  const auto &reservations() const {
    return m_extents;
  }
  auto &reservations() {
    return m_extents;
  }

  auto id() const {
    return m_page.id();
  }
  Page_t &page() {
    return m_page;
  }
  const Page_t &page() const {
    return m_page;
  }
};
}

#endif // FS_SEGMENT_H
