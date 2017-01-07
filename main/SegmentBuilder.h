#ifndef PROJECT_SEGMENT_BUILDER_H
#define PROJECT_SEGMENT_BUILDER_H

#include "../fs/Line.h"
#include "../page/FilePage.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include "../shared/entities.h"
#include <algorithm>
#include <bitset>

namespace page {

template <typename Meta_t>
class SegmentBuilder {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using Line_t = db::Line<Table_t, hash_t>;
  static constexpr size_t lines = Meta_t::extent_lines();

private:
  const FilePageMeta m_segment;

  std::bitset<lines> m_present;
  db::segment::id m_greatest;
  size_t m_current;

public:
  explicit SegmentBuilder(const FilePageMeta &segment)
      : m_segment(segment), m_present(), m_greatest(db::segment::START_ID), m_current(0) {
  }

public:
  void operator()(const db::LineMeta &line) {
    m_present[m_current++] = bool(line);
    m_greatest = std::max(line.id, m_greatest);
  }

  db::Segment<Meta_t> build() {
    // TODO make Line_size dynamic(based o:n version)
    // TODO calculate no of lines and have a linked bitset list if there are
    // more lines present in file than in lines() constexpr-
    // should work with present set and reservation set

    FilePage<Meta_t> page(m_segment);

    db::PresentSet<Meta_t> ps{m_present};
    return db::Segment<Meta_t>{std::move(page), std::move(ps)};
  }
};
}
#endif
