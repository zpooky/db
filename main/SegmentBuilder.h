#ifndef PROJECT_SEGMENT_BUILDER_H
#define PROJECT_SEGMENT_BUILDER_H

#include "../page/FilePage.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include "../shared/PageRange.h"
#include "ExtentsBuilder.h"
#include <vector>

namespace page {

template <typename Meta_t>
class SegmentBuilder {
private:
  static constexpr size_t lines = Meta_t::extent_lines();

private:
  const FilePageMeta m_meta;
  db::ExtentsBuilder<lines> m_extents;

public:
  explicit SegmentBuilder(const FilePageMeta &meta)
      : m_meta(meta), m_extents() {
  }

  SegmentBuilder(const SegmentBuilder &&) = delete;
  SegmentBuilder(const SegmentBuilder &) = delete;

public:
  void operator()(const FilePageMeta &, const db::LineMeta &line) {
    m_extents.next(bool(line));
  }

  db::Segment<Meta_t> build() {
    // TODO make Line_size dynamic(based o:n version)
    // TODO calculate no of lines and have a linked bitset list if there are
    // more lines present in file than in lines() constexpr-
    // should work with present set and reservation set

    // TODO test FIFO order of extents
    using PresentSet = db::PresentSet<lines>;
    FilePage<Meta_t> page(m_meta);

    auto &builders = m_extents.builders();
    std::vector<db::Extent<Meta_t>> extents;
    page::position start(0);
    db::segment::id id(m_meta.id);
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
