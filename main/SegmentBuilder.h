#ifndef PROJECT_SEGMENT_BUILDER_H
#define PROJECT_SEGMENT_BUILDER_H

#include "../page/FilePage.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include "../shared/PageRange.h"
#include <bitset>
#include <deque>
#include <vector>

namespace {
template <size_t capacity>
class ExtentBuilder {
private:
  using Bitset_t = std::bitset<capacity>;
  Bitset_t m_present;
  size_t m_current;

public:
  explicit ExtentBuilder() : m_present(0), m_current(0) {
  }

  ExtentBuilder(ExtentBuilder<capacity> &&o)
      : m_present{std::move(o.m_present)}, m_current{o.m_current} {
  }

  ExtentBuilder(const ExtentBuilder<capacity> &o)
      : m_present{o.m_present}, m_current{o.m_current} {
  }

  void next(bool present) {
    m_present[m_current++] = present;
  }

  bool is_full() const {
    return m_current == capacity;
  }

  size_t lines() const {
    return m_current;
  }

  const Bitset_t &present() const & {
    return m_present;
  }

  Bitset_t &present() & {
    return m_present;
  }

  Bitset_t &&present() && {
    return std::move(m_present);
  }
};
}
namespace page {

template <size_t lines>
class ExtentsBuilder {
private:
  using ExtentBuilder_t = ExtentBuilder<lines>;

private:
  std::deque<ExtentBuilder_t> m_extents;
  ExtentBuilder_t *m_current;

public:
  ExtentsBuilder() : m_extents(), m_current(nullptr) {
  }

private:
  bool is_full() const {
    return m_current == nullptr || m_current->is_full();
  }

public:
  void next(bool present) {
    if (is_full()) {
      m_extents.emplace_back();
      m_current = &m_extents.back();
    }
    m_current->next(present);
  }

  std::deque<ExtentBuilder_t> &builders() {
    return m_extents;
  }
};

template <typename Meta_t>
class SegmentBuilder {
private:
  static constexpr size_t lines = Meta_t::extent_lines();

private:
  const FilePageMeta m_segment;
  ExtentsBuilder<lines> m_extents;

public:
  explicit SegmentBuilder(const FilePageMeta &segment)
      : m_segment(segment), m_extents() {
  }

public:
  void operator()(const db::LineMeta &line) {
    m_extents.next(bool(line));
  }

  db::Segment<Meta_t> build() {
    // TODO make Line_size dynamic(based o:n version)
    // TODO calculate no of lines and have a linked bitset list if there are
    // more lines present in file than in lines() constexpr-
    // should work with present set and reservation set

    // TODO test FIFO order of extents
    using PresentSet = db::PresentSet<lines>;
    FilePage<Meta_t> page(m_segment);

    auto &builders = m_extents.builders();
    std::vector<db::Extent<Meta_t>> extents;
    page::position start(0);
    db::segment::id id(m_segment.id);
    for (auto &extent : builders) {
      auto extent_lines(extent.lines());
      db::PageRange range(start, extent_lines);
      extents.emplace_back(id, PresentSet(extent.present(), range));
      start = page::position(start + extent_lines);
    }

    return db::Segment<Meta_t>{std::move(page), db::Extents<Meta_t>(extents)};
  }
};
}
#endif
