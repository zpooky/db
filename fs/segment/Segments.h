//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTS_H
#define PROJECT_SEGMENTS_H

#include "../../config/Configuration.h"
#include <atomic>
#include "../../shared/Assertions.h"
#include "../../shared/vfs.h"
#include "Segment.h"
#include "../../collection/List.h"
#include "Reservations.h"
#include <functional>

namespace db {
    namespace fs {
        struct Context {
            const db::Directory root;
            const db::segment::index_type start;

            Context(const Directory &p_root, db::segment::index_type p_start) : root(p_root),
                                                                                start{p_start} { }
        };

        template<typename T_Table>
        class ColSegments {
        private:
            using index_type = db::segment::index_type;
            std::atomic<index_type> m_seg_counter;
            const Directory m_root;
            sp::List<Reservations<T_Table>> m_vector;

            SegmentJournalThread<hash::crc32> m_journal_thread;
            SegmentJournal<T_Table, hash::crc32> m_journal;
        public:
            ColSegments(index_type p_index, const Directory &p_root) : m_seg_counter{p_index},
                                                                       m_root{p_root.cd("segment")},
                                                                       m_journal(
                                                                               m_root.cd(Filename("segment.journal")),
                                                                               m_journal_thread, 0l) {
                db::assert_is_table<T_Table>();
                db::vfs::mkdir(m_root);
            }

            Reservation reserve() {
                auto &resvs = free();
                return {0};
            }

        private:
            Reservations<T_Table> make() {
                using namespace db::fs::internal;
                const auto line_size = Line_size<T_Table>::value();
                SegmentFileInit init{m_root, line_size, 1024l}; // TODO
                SegmentFileInitJournal<T_Table> sfj{init, m_journal};
                auto segment = sfj.create(m_seg_counter++);
                return {segment};
            }

//            static Reservations<T_Table> makex() {
//                SegmentFile sf{File(""), 1l, 1l};
//                Segment<T_Table> segment{sf};
//                return {segment};
//            }

            Reservations<T_Table> &free() {
//                std::function<bool()>
                auto p = [](const Reservations<T_Table> &r) -> bool {
                    return false;
                };

                auto f = [&]() -> Reservations<T_Table> {
                    return this->make();
                };

                return m_vector.find(p, f);
            }
        };

        template<typename T_Table>
        class Segments {
        private:
            const db::Directory m_root;
            ColSegments<T_Table> m_segments;
        public:
            Segments(const Context &ctx) : m_root{ctx.root.cdx(T_Table::table_name())},
                                           m_segments{ctx.start, m_root} {
                db::assert_is_table<T_Table>();
                db::vfs::mkdir(m_root);
            }

            Segments(const Segments &o) = delete;

            Reservation reserve();

        private:

        };

        template<typename T_table>
        Reservation Segments<T_table>::reserve() {
            return m_segments.reserve();
        }
    }
}
#endif //PROJECT_SEGMENTS_H
