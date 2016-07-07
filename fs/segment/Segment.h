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
#include "format/Format.h"
#include "SegmentFile.h"
#include "PresentSet.h"


namespace db {
    namespace fs {

        template<typename T_Meta>
        class Segment {
        private:
            using hash_algh = typename T_Meta::hash_algh;
            SegmentFile m_file;
            PresentSet<T_Meta> m_lines;
            //State
        public:
            explicit Segment(SegmentFile &&file, PresentSet<T_Meta> &&lines) :
                    m_file{std::move(file)},
                    m_lines{std::move(lines)} {
//                db::assert_is_meta<T_Meta>();
            }

            Segment(Segment<T_Meta> &&o) :
                    m_file{std::move(o.m_file)},
                    m_lines{std::move(o.m_lines)} {
//                db::assert_is_meta<T_Meta>();
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE, hash_algh> &line) {
                return true;
            }

            const PresentSet<T_Meta>& present_set();
        };

        template<typename T_Meta>
        const PresentSet<T_Meta> &Segment<T_Meta>::present_set() {
            return m_lines;
        }


        namespace internal {
            template<typename T_Meta>
            class SegmentFileInitJournal {
            private:
                using T_Table = typename T_Meta::Table;
                const Directory &m_root;
                SegmentJournal<T_Meta> &m_journal;
            public:
                explicit SegmentFileInitJournal(const Directory &root, SegmentJournal<T_Meta> &journal) :
                        m_root{root},
                        m_journal{journal} {
//                    db::assert_is_table<T_Table>();
                }

                SegmentFileInitJournal(const SegmentFile &) = delete;

                SegmentFileInitJournal(SegmentFile &&) = delete;

                Segment<T_Meta> create(db::segment::index_type idx) {
                    auto id = m_journal.start(T_Table::table_name(), idx);
                    Filename seg_name{db::Segment_name<T_Table>::name(idx)};
                    typename db::segment::Format::latest<T_Meta> init{m_root};
                    PresentSet<T_Meta> p{};
                    Segment<T_Meta> result{init.create(seg_name), std::move(p)};
//                m_journal.prpare(id);
                    m_journal.commit(id);
                    return result;
                }
            };

        }


    }
}


#endif //FS_SEGMENT_H
