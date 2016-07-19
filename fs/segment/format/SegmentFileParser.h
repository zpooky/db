//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_SEGMENTFILEPARSER_H
#define PROJECT_SEGMENTFILEPARSER_H

#include "../Segment.h"

namespace db {
    namespace fs {
        template<typename T_Meta>
        class SegmentFileParser {
        private:
            const File m_seg_file;
        public:
            explicit SegmentFileParser(const File &seg_file) :
                    m_seg_file(seg_file) {
            }

            Segment<T_Meta> parse() {
                // TODO parse version
                db::segment::version v = 1u;
                auto parser = db::segment::Format::parser<T_Meta>(v);
                // TODO make Line_size dynamic(based o:n version)
                // TODO calculate no of lines and have a linked bitset list if there are more lines present in file than in lines() constexpr-
                // should work with present set and reservation set
                SegmentFile sf{m_seg_file, Line_size<T_Meta>::value(), T_Meta::lines()};
                db::segment::id segment_id = parser.get_id(m_seg_file);
                return Segment<T_Meta>{segment_id, std::move(sf), parser.parse(m_seg_file)};
            }
        };
    }
}

#endif //PROJECT_SEGMENTFILEPARSER_H
