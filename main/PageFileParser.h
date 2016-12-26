//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_SEGMENTFILEPARSER_H
#define PROJECT_SEGMENTFILEPARSER_H

#include "../segment/Segment.h"
#include "../shared/entities.h"
#include "../page/format/Format.h"
#include "../page/io/FilePageMeta.h"
#include "../page/FilePage.h"

namespace page {
template <typename Meta_t>
class PageFileParser {
private:
  const db::File m_segment;

public:
  explicit PageFileParser(const db::File &segment) : m_segment(segment) {
  }

  db::fs::Segment<Meta_t> parse() {
    // TODO parse version
    db::segment::version v(1u);
    db::table::version table_version(1u);
    auto parser = page::Format::parser<Meta_t>(v);
    // TODO make Line_size dynamic(based o:n version)
    // TODO calculate no of lines and have a linked bitset list if there are
    // more lines present in file than in lines() constexpr-
    // should work with present set and reservation set
    db::segment::id sid = parser.get_id(m_segment);
    FilePageMeta meta{sid, m_segment, db::fs::Line_size<Meta_t>::value(),
                   Meta_t::extent_lines(), table_version};

    FilePage<Meta_t> page(std::move(meta));
    return db::fs::Segment<Meta_t>{std::move(page), parser.parse(m_segment)};
  }
};
}
#endif // PROJECT_SEGMENTFILEPARSER_H
