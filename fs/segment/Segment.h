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
            FileWriter m_file;
            size_t m_line_size;
            size_t m_number;
        public:

            SegmentFile(const File &file, size_t line_size, size_t number) : m_file{file},
                                                                             m_line_size{line_size},
                                                                             m_number{number} {
            }

            SegmentFile(const SegmentFile &) = delete;

            SegmentFile(SegmentFile &&o) : m_file{std::move(o.m_file)},
                                           m_line_size{o.m_line_size},
                                           m_number{o.m_number} {

            }
        };

        template<typename t_Table>
        class Segment {
        private:
            SegmentFile m_file;
            PresentSet m_lines;
            //State
        public:
            Segment(Segment<t_Table> &&o) : m_file{std::move(o.m_file)}, m_lines{std::move(o.m_lines)} {
                db::assert_is_table<t_Table>();
            }

            Segment(SegmentFile &&file) : m_file{std::move(file)} {
                db::assert_is_table<t_Table>();
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE> &line) {
                return true;
            }
        };

        namespace internal {
            class SegmentFileInit {
            private:
                const Directory m_root;
                const size_t m_line_size;
                const size_t m_lines;
            public:
                SegmentFileInit(const Directory &root, size_t line_size, size_t lines) : m_root{root},
                                                                                         m_line_size{line_size},
                                                                                         m_lines{lines} {
                }

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

                Segment<T_Table> create(db::segment::index_type);
            };

            template<typename T_Table>
            Segment<T_Table> SegmentFileInitJournal<T_Table>::create(db::segment::index_type idx) {
                auto id = m_journal.start(T_Table::table_name(), idx);
                auto seg_name = db::Segment_name<T_Table>::name(idx);
                Segment<T_Table> result{m_init.create(seg_name)};
//                m_journal.prepare(id);
                m_journal.commit(id);
                return result;
            }
        }


    }
}


#endif //FS_SEGMENT_H
