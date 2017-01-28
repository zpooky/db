#ifndef PROJECT_SEGMENT_BUILDER_H
#define PROJECT_SEGMENT_BUILDER_H

#include "../page/FilePage.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include <bitset>
#include <deque>
#include <vector>

namespace page {
template <size_t lines>
class ExtentBuilder {
private:
  std::bitset<lines> m_present;
  size_t m_current;

public:
  explicit ExtentBuilder() : m_present(0), m_current(0) {
  }

  ExtentBuilder(ExtentBuilder<lines> &&o)
      : m_present{std::move(o.m_present)}, m_current{o.m_current} {
  }

  ExtentBuilder(const ExtentBuilder<lines> &o)
      : m_present{o.m_present}, m_current{o.m_current} {
  }

  void next(bool present) {
    m_present[m_current++] = present;
  }

  bool is_full() const {
    return m_current == lines;
  }
  const std::bitset<lines> &present() const & {
    return m_present;
  }
  std::bitset<lines> &present() & {
    return m_present;
  }
  std::bitset<lines> &&present() && {
    return std::move(m_present);
  }
};

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
    FilePage<Meta_t> page(m_segment);

    using Extent_t = db::Extent<Meta_t>;
    auto builders = m_extents.builders();
    std::vector<Extent_t> extents;
    for (auto &b : builders) {
      db::PresentSet<lines> ps(b.present());
      extents.emplace_back(m_segment.id, std::move(ps));
    }

    return db::Segment<Meta_t>{std::move(page), db::Extents<Meta_t>(extents)};
  }
};
}
#endif
