//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../../shared/entities.h"
#include "../../shared/Assertions.h"
#include "../FileWriter.h"

/*
 * Header:
 * .----------------------.
 * |Table name | Line size|
 * `---------------------
 */

namespace db {
    namespace fs {
        class SegmentFile;
        namespace impl {
            class SegmentFileInit {
            private:
                const File m_file;
            public:
                SegmentFile create(size_t line_size, size_t number) const;
            };
        }

        class SegmentFile {
        private:
            const FileWriter m_file;
            const size_t m_line_size;
            const size_t m_number;
        public:

            SegmentFile(const File &file, size_t line_size, size_t number) : m_file{file},
                                                                                    m_line_size{line_size},
                                                                                    m_number{number} {
            }


        };

        template<typename t_Table>
        class Segment {
        private:
            SegmentFile m_file;
            //State
        public:
            Segment(const SegmentFile &file) : m_file{file} {
                db::assert_is_table<t_Table>();
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE> &line) {
                return true;
            }
        };


    }
}


#endif //FS_SEGMENT_H
