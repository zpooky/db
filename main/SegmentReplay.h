#ifndef PROJECT_SEGMENT_BUILDER_H
#define PROJECT_SEGMENT_BUILDER_H

#include "../page/FilePage.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include "../shared/PageRange.h"
#include "ExtentsBuilder.h"
#include "Util.h"
#include <vector>

namespace page {

template <typename Meta_t>
class SegmentReplay {
private:
  static constexpr size_t lines = Meta_t::extent_lines();

private:
  const FilePageMeta m_meta;
  db::ExtentsBuilder<lines> m_extents;

public:
  //,SegmentFileHint(Size)
  explicit SegmentReplay(const FilePageMeta &meta) : m_meta(meta), m_extents() {
  }

  SegmentReplay(const SegmentReplay &&) = delete;
  SegmentReplay(const SegmentReplay &) = delete;

public:
  void operator()(const FilePageMeta &, const db::LineMeta &line) {
    m_extents.next(bool(line));
  }

  db::Segment<Meta_t> build() {
    // TODO make Line_size dynamic(based o:n version)

    auto &builders = m_extents.builders();
    return db::Segment<Meta_t>{
        m_meta.id, FilePage<Meta_t>(m_meta),
        db::Extents<Meta_t>(db::segment::extents<Meta_t>(builders))};
  }
};
}
#endif
