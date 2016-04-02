//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../../shared/fs.h"
#include "../../shared/Assertions.h"
#include "../File.h"

namespace db {
    namespace fs {
        class SegmentFile {
        private:
            const size_t m_filesize;
            const File m_file;
        public:
            SegmentFile(const std::string &file, size_t filesize) : m_file{file}, m_filesize{filesize} {
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
            template <size_t SIZE>
            bool write(const Reservation& r, const Line<SIZE> &line){
                return true;
            }
        };


    }
}


#endif //FS_SEGMENT_H
