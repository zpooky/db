//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_SEGMENTFILEPARSER_H
#define PROJECT_SEGMENTFILEPARSER_H

#include "../page/FilePage.h"
#include "../page/format/Format.h"
#include "../page/io/FilePageMeta.h"
#include "../segment/Segment.h"
#include "../shared/entities.h"

namespace page {
template <typename Meta_t>
class PageFileParser {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;

private:
  const db::File m_segment;

public:
  explicit PageFileParser(const db::File &segment) : m_segment(segment) {
  }

  db::Segment<Meta_t> parse() {
    // TODO parse version
    db::segment::version v(1u);
    db::table::version table_version(1u);
    auto parser = page::Format::parser<Meta_t>(v);
    // TODO make Line_size dynamic(based o:n version)
    // TODO calculate no of lines and have a linked bitset list if there are
    // more lines present in file than in lines() constexpr-
    // should work with present set and reservation set
    db::segment::id sid = db::Segment_name::id(m_segment.filename());
    constexpr auto line_size = db::Line<Table_t, hash_t>::size();
    constexpr auto extent_lines = Meta_t::extent_lines();
    FilePageMeta meta{sid, m_segment, line_size, extent_lines, table_version};

    FilePage<Meta_t> page(std::move(meta));
    return db::Segment<Meta_t>{std::move(page), parser.parse(m_segment)};
  }
};
}
#endif // PROJECT_SEGMENTFILEPARSER_H
