#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "Extents.h"

namespace db {

template <typename Meta_t>
class Segment {
private:
  using Page_t = typename Meta_t::Page;
  using Extents_t = Extents<Meta_t>;
  using Table_t = typename Meta_t::latest;

private:
  Extents_t m_extents;
  Page_t m_page;
  const db::segment::id m_id;
  // State
public:
  explicit Segment(db::segment::id id, Page_t &&page, Extents_t &&extents)
      : m_extents{std::move(extents)}, m_page{std::move(page)}, m_id(id) {
  }

  Segment(Segment<Meta_t> &&o)
      : m_extents{std::move(o.m_extents)}, m_page{std::move(o.m_page)},
        m_id(o.m_id) {
  }

  Segment(const Segment &) = delete;

  ~Segment() {
  }

  sp::Maybe<Reservation<Table_t>> reserve() {
    auto res = m_extents.reserve();
    if (res) {
      Reservation<Table_t> r(m_id, res.get());
      return sp::Maybe<Reservation<Table_t>>{r};
    }
    return sp::Maybe<Reservation<Table_t>>{};
  }

  bool has_free() const {
    return m_extents.has_free();
  }

  auto id() const {
    return m_id;
  }

  Page_t &page() {
    return m_page;
  }

  const Page_t &page() const {
    return m_page;
  }
};
} // namespace db

#endif // FS_SEGMENT_H
