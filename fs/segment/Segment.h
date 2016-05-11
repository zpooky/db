//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../../shared/entities.h"
#include "../../shared/Assertions.h"
#include "../../config/Configuration.h"
#include "../FileWriter.h"
#include "SegmentJournal.h"

/*
 * Header:
 * size in bytes:
 * |db::table::name::length|unsigned byte|unsigned byte|unsigned short|variable length|
 *
 * header fields:
 * .-------------------------------------------------------------------------------.
 * |Table name|Line size|partition type|partition meta data length|meta data|
 * `-------------------------------------------------------------------------------`
 *
 * partition type:
 * none                     : 0
 * day based partitioning   : 1
 *
 * endianess - ??
 */

namespace db {
    namespace fs {
        class PresentSet {
        private:
        };

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
            PresentSet m_lines;
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

        namespace impl {
            class SegmentFileInit {
            private:
                const Directory m_root;
                const size_t m_line_size;
                const size_t m_lines;
            public:
                SegmentFile create(const Filename &file) const;

            };

            template<typename T_Table>
            class SegmentFileInitJournal {
            private:
                SegmentFileInit &m_init;
                SegmentJournal<T_Table> &m_journal;
            public:
                SegmentFileInitJournal(SegmentFileInit &init, SegmentJournal<T_Table> &journal) : m_init{init},
                                                                                                  m_journal{journal} {
                    db::assert_is_table<T_Table>();
                }

                Segment create(db::segment::index_type);
            };

            template<typename T_Table>
            Segment SegmentFileInitJournal<T_Table>::create(db::segment::index_type idx) {
                auto id = m_journal.start(m_init.get_file());
                SegmentFile result = m_init.create(Line_size<T_Table>::value(), number);
                m_journal.prepare(id);
                m_journal.commit(id);
                return result;
            }
        }


    }
}


#endif //FS_SEGMENT_H
