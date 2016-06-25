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
            using index_type = db::segment::index_type;
            const db::Directory root;
            const index_type start;

            explicit Context(const Directory &p_root, index_type p_start) : root(p_root),
                                                                            start{p_start} { }

        };

        template<typename T_Meta>
        class SegmentFileFactory {
        private:
            using index_type = db::segment::index_type;
            using hash_algh = typename T_Meta::hash_algh;
            std::atomic<index_type> m_seg_counter;
            const Directory m_root;
            // --
            SegmentJournalThread<hash_algh> m_journal_runnable;
            SegmentJournal<T_Meta> m_journal;
            std::thread m_journal_thread;
        public:
            explicit SegmentFileFactory(index_type p_index, const Directory &p_root) :
                    m_seg_counter{p_index},
                    m_root{p_root},
                    m_journal(p_root.cd(Filename("segment.journal")), m_journal_runnable, 0l) {
            }

            SegmentFileFactory(SegmentFileFactory<T_Meta> &&o) :
                    m_seg_counter{0}, m_root{o.m_root},
                    m_journal(m_root.cd(Filename("segment.journal")), m_journal_runnable, 0l)
            // m_seg_counter{std::move(o.m_seg_counter)},
            //,
            //        m_journal_runnable{std::move(o.m_journal_thread)},
            //       m_journal{std::move(o.m_journal)},
            //      m_journal_thread{std::move(o.m_journal_thread)}
            {
                //TODO
            }

            ~SegmentFileFactory() {
                m_journal_runnable.interrupt();
                if (m_journal_thread.joinable()) {
                    m_journal_thread.join();
                }
            }

            Segment<T_Meta> operator()() {
                using namespace db::fs::internal;
                const auto line_size = Line_size<T_Meta>::value();
                SegmentFileInit init{m_root, line_size, T_Meta::lines()};
                SegmentFileInitJournal<T_Meta> sfj{init, m_journal};
                return sfj.create(m_seg_counter++);
            }
        };

        template<typename T_Meta>
        class ColSegments {
        private:
            using T_Table = typename T_Meta::Table;
            using index_type = db::segment::index_type;
            // --
            SegmentFileFactory<T_Meta> m_factory;
            sp::List<Reservations<T_Meta>> m_vector;

        public:
            ColSegments(SegmentFileFactory<T_Meta> &&factory) :
                    m_factory{std::move(factory)} {
//                db::assert_is_context<T_Meta>();
            }

            ColSegments(ColSegments<T_Meta> &&o) :
                    m_factory{std::move(o.m_factory)},
                    m_vector{std::move(o.m_vector)} {

            }

            ~ColSegments() {
            }

            Reservation reserve() {
                auto &reservations = free();
                return Reservation{0};
            }

        private:

            Reservations<T_Meta> &free() {
                auto p = [](const Reservations<T_Meta> &r) -> bool {
                    return false;
                };

                auto f = [&]() {
                    return Reservations<T_Meta>{this->m_factory()};
                };

                return m_vector.find(p, f);
            }

        public:

            static ColSegments apply(const Directory &p_root) {
                //TODO
                SegmentFileFactory<T_Meta> sff{1l, db::vfs::mkdir(p_root.cd("segment"))};
                return {std::move(sff)};
            }
        };

        template<typename T_Meta>
        class Segments {
        private:
            using T_Table = typename T_Meta::Table;
            const db::Directory m_root;
            ColSegments<T_Meta> m_segments;
        public:
            explicit Segments(const Context &ctx) :
                    m_root{db::vfs::mkdir(ctx.root.cdx(T_Table::table_name()))},
                    m_segments{ColSegments<T_Meta>::apply(m_root)} {
//                db::assert_is_context<T_Table>();
            }

            Segments(Segments<T_Meta> &&o) :
                    m_root{o.m_root},
                    m_segments{std::move(o.m_segments)} {

            }

            Segments(const Segments<T_Meta> &) = delete;

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
