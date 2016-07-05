//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_SEGMENTFILE_H
#define PROJECT_SEGMENTFILE_H


#include <cstddef>
#include <utility>
#include "../FileWriter.h"

namespace db {
    namespace fs {
        class SegmentFile {
        private:
            FileWriter m_file;
            size_t m_line_size;
            size_t m_number;
        public:

            SegmentFile(const File &file, size_t line_size, size_t number) :
                    m_file{file},
                    m_line_size{line_size},
                    m_number{number} {
            }

            SegmentFile(const SegmentFile &) = delete;

            SegmentFile(SegmentFile &&o) :
                    m_file{std::move(o.m_file)},
                    m_line_size{o.m_line_size},
                    m_number{o.m_number} {

            }

            const File &file() const {
                return m_file.file();
            }
        };
    }
}
#endif //PROJECT_SEGMENTFILE_H
