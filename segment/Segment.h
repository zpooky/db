//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../shared/entities.h"
#include "../shared/Assertions.h"
#include "../config/Configuration.h"
#include "../fs/FileWriter.h"
#include "SegmentJournal.h"
#include "format/Format.h"
#include "PresentSet.h"


namespace db {
    namespace fs {

        template<typename Meta_t>
        class Segment {
        private:
            const db::segment::id m_id;
            using hash_algh = typename Meta_t::hash_algh;
            SegmentFile m_file;
            PresentSet<Meta_t> m_lines;
            //State
        public:
            explicit Segment(db::segment::id id, SegmentFile &&file, PresentSet<Meta_t> &&lines) :
                    m_id{id},
                    m_file{std::move(file)},
                    m_lines{std::move(lines)} {
//                db::assert_is_meta<Meta_t>();
            }

            Segment(Segment<Meta_t> &&o) :
                    m_id{o.m_id},
                    m_file{std::move(o.m_file)},
                    m_lines{std::move(o.m_lines)} {
//                db::assert_is_meta<Meta_t>();
            }

            Segment(const Segment &) = delete;

            auto get_id() const -> decltype(m_id) {
                return m_id;
            }

            template<size_t SIZE>
            bool write(const Reservation &r, const Line<SIZE, hash_algh> &line) {
                return true;
            }

            const PresentSet<Meta_t> &present_set();
        };

        template<typename Meta_t>
        const PresentSet<Meta_t> &Segment<Meta_t>::present_set() {
            return m_lines;
        }


        namespace internal {
            template<typename Meta_t>
            class SegmentFileInitJournal {
            private:
                using Table_t = typename Meta_t::Table;
                using hash_algh = typename Meta_t::hash_algh;
                const Directory &m_root;
                SegmentJournal<hash_algh> &m_journal;
            public:
                explicit SegmentFileInitJournal(const Directory &root, SegmentJournal<hash_algh> &journal) :
                        m_root(root),
                        m_journal{journal} {
//                    db::assert_is_table<Table_t>();
                }

                SegmentFileInitJournal(const SegmentFile &) = delete;

                SegmentFileInitJournal(SegmentFile &&) = delete;

                Segment<Meta_t> create(db::segment::id segment_id) {
                    auto id = m_journal.start(Table_t::table_name(), segment_id);
                    Filename seg_name{db::Segment_name<Table_t>::name(segment_id)};

                    typename db::segment::Format::latest<Meta_t> init{m_root};
                    PresentSet<Meta_t> p{};

                    Segment<Meta_t> result{segment_id, init.create(seg_name), std::move(p)};
                    m_journal.commit(id);
                    return result;
                }
            };

        }


    }
}


#endif //FS_SEGMENT_H
