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

        template<typename T_Meta>
        class Segment {
        private:
            using hash_algh = typename T_Meta::hash_algh;
            SegmentFile m_file;
            PresentSet m_lines;
            //State
        public:
            Segment(Segment<T_Meta> &&o) : m_file{std::move(o.m_file)}, m_lines{std::move(o.m_lines)} {
//                db::assert_is_meta<T_Meta>();
            }

            explicit Segment(SegmentFile &&file) : m_file{std::move(file)} {
//                db::assert_is_meta<T_Meta>();
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE, hash_algh> &line) {
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

                SegmentFile create(const Filename &);

            };

            template<typename T_Meta>
            class SegmentFileInitJournal {
            private:
                using T_Table = typename T_Meta::Table;
                SegmentFileInit &m_init;
                SegmentJournal<T_Meta> &m_journal;
            public:
                SegmentFileInitJournal(SegmentFileInit &init, SegmentJournal<T_Meta> &journal) : m_init{init},
                                                                                                 m_journal{journal} {
//                    db::assert_is_table<T_Table>();
                }

                Segment<T_Meta> create(db::segment::index_type idx) {
                    auto id = m_journal.start(T_Table::table_name(), idx);
                    Filename seg_name{db::Segment_name<T_Table>::name(idx)};
                    Segment<T_Meta> result{m_init.create(seg_name)};
//                m_journal.prpare(id);
                    m_journal.commit(id);
                    return result;
                }
            };

        }


    }
}


#endif //FS_SEGMENT_H
