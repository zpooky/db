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

            explicit Context(const Directory &p_root, db::segment::index_type p_start) : root(p_root),
                                                                                         start{p_start} { }
        };

        template<typename T_Meta>
        class ColSegments {
        private:
            using T_Table = typename T_Meta::Table;
            using index_type = db::segment::index_type;
            std::atomic<index_type> m_seg_counter;
            const Directory m_root;
            sp::List<Reservations<T_Meta>> m_vector;

            SegmentJournalThread<hash::crc32> m_journal_runnable;
            SegmentJournal<T_Table, hash::crc32> m_journal;
            std::thread m_journal_thread;

        public:
            explicit
            ColSegments(index_type p_index, const Directory &p_root) : m_seg_counter{p_index},
                                                                       m_root{db::vfs::mkdir(p_root.cd("segment"))},
                                                                       m_journal(m_root.cd(Filename("segment.journal")),
                                                                                 m_journal_runnable, 0l)
//                                                                        ,
//                                                                       m_journal_thread{m_journal_runnable}
            {
//                db::assert_is_context<T_Meta>();
            }

//            ColSegments(ColSegments<T_Table> && o) : m_seg_counter{o.m_seg_counter},
//                                                     m_root{o.m_root},
//                                                     m_vector{std::move(o.m_vector)},
//                                                     m_journal_runnable{std::move(o.m_journal_thread)},
//                                                     m_journal{std::move(o.m_journal)},
//                                                     m_journal_thread{std::move(o.m_journal_thread)}
//
//            {
//
//            }

            ~ColSegments() {
                m_journal_runnable.interrupt();
                if (m_journal_thread.joinable()) {
                    m_journal_thread.join();
                }
            }

            Reservation reserve() {
                auto &resvs = free();
                return Reservation{0};
            }

        private:
            Reservations<T_Meta> make() {
                using namespace db::fs::internal;
                const auto line_size = Line_size<T_Table>::value();
                SegmentFileInit init{m_root, line_size, T_Meta::lines()};
                SegmentFileInitJournal<T_Table> sfj{init, m_journal};
                return Reservations<T_Meta>{sfj.create(m_seg_counter++)};
            }

            Reservations<T_Meta> &free() {
                auto p = [](const Reservations<T_Meta> &r) -> bool {
                    return false;
                };

                auto f = [&]() {
                    return this->make();
                };

                return m_vector.find(p, f);
            }
        };

        template<typename T_Meta>
        class Segments {
        private:
            using T_Table = typename T_Meta::Table;
            const db::Directory m_root;
            ColSegments<T_Meta> m_segments;
        public:
            explicit Segments(const Context &ctx) : m_root{db::vfs::mkdir(ctx.root.cdx(T_Table::table_name()))},
                                                    m_segments{ctx.start, m_root} {
//                db::assert_is_context<T_Table>();
            }

            Segments(const Segments<T_Meta> &o) = delete;

            Reservation reserve();

        private:

        };

        template<typename T_Meta>
        Reservation Segments<T_Meta>::reserve() {
            return m_segments.reserve();
        }
    }
}
#endif //PROJECT_SEGMENTS_H
