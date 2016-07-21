//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTS_H
#define PROJECT_SEGMENTS_H

#include "../config/Configuration.h"
#include <atomic>
#include "../shared/Assertions.h"
#include "../shared/vfs.h"
#include "../shared/conversions.h"
#include "Segment.h"
#include "../collection/List.h"
#include "Reservations.h"
#include <functional>
#include <vector>
#include <algorithm>
#include "format/SegmentFileParser.h"
#include "Context.h"

namespace db {
    namespace fs {
        template<typename T_Meta>
        class SegmentFileFactory {
        private:
            using segment_id = db::segment::id;
            using hash_algh = typename T_Meta::hash_algh;
            std::atomic<segment_id> m_seg_counter;
            const Directory m_root;
            // --
            SegmentJournal<hash_algh> &m_journal;
        public:
            explicit SegmentFileFactory(Context<hash_algh> &ctx, segment_id p_index, const Directory &p_root) :
                    m_seg_counter{p_index},
                    m_root(p_root),
                    m_journal(ctx.m_journal) { }

            SegmentFileFactory(SegmentFileFactory<T_Meta> &&o) :
                    m_seg_counter{o.m_seg_counter.load()},
                    m_root(o.m_root),
                    m_journal(o.m_journal) {
            }

            ~SegmentFileFactory() {
            }

            Segment<T_Meta> operator()() {
                using namespace db::fs::internal;
                SegmentFileInitJournal<T_Meta> sfj{m_root, m_journal};
                return sfj.create(m_seg_counter++);
            }
        };


        template<typename T_Meta>
        class ColSegments {
        private:
            using T_Table = typename T_Meta::Table;
            using hash_algh = typename T_Meta::hash_algh;
            using segment_id = db::segment::id;
            // --
            SegmentFileFactory<T_Meta> m_factory;
            sp::List<Reservations<T_Meta>> m_vector;

        public:
            explicit ColSegments(SegmentFileFactory<T_Meta> &&factory, const std::vector<File> &p_segments) :
                    m_factory{std::move(factory)} {
                for (const auto &file : p_segments) {
                    SegmentFileParser<T_Meta> parser(file);
                    m_vector.push_front(Reservations<T_Meta>{parser.parse()});
                }
//                db::assert_is_context<T_Meta>();
            }

            ColSegments(ColSegments<T_Meta> &&o) :
                    m_factory{std::move(o.m_factory)},
                    m_vector{std::move(o.m_vector)} {

            }

            ~ColSegments() {
            }

            Reservation reserve() {
                while (true) {
                    auto &reservations = free();
                    auto maybe_res = reservations.reserve();
                    if (maybe_res.is_present()) {
                        return maybe_res.get();
                    }
                }
            }

        private:

            Reservations<T_Meta> &free() {
                auto p = [](const Reservations<T_Meta> &r) -> bool {
                    return r.has_free();
                };

                auto f = [&]() {
                    return Reservations<T_Meta>{this->m_factory()};
                };

                return m_vector.find(p, f);
            }

        public:
            class DD {
            private:
                const Directory &m_root;
            public:
                explicit DD(const Directory &root) :
                        m_root(root) {
                }

                DD(const DD &) = delete;

                std::tuple<db::segment::id, std::vector<File>> operator()() {
                    std::vector<File> segments = files(m_root);

                    std::vector<db::segment::id> num_segments(segments.size());
                    std::transform(segments.begin(), segments.end(), num_segments.begin(), [](const auto &file) {
                        auto fname = file.filename();
                        return db::to<db::segment::id>(fname.name.c_str());
                    });

                    auto max_it = std::max_element(num_segments.begin(), num_segments.end());
                    auto max = max_it != num_segments.end() ? *max_it : 1l;

                    return std::make_tuple(max, segments);
                }
            };

            static ColSegments<T_Meta> apply(Context<hash_algh> &ctx, const Directory &p_root) {
                auto seg_root = db::vfs::mkdir(p_root.cd("segment"));

                DD d(seg_root);

                db::segment::id seg_cnt;
                std::vector<File> segments;
                std::tie(seg_cnt, segments) = d();

                seg_cnt = db::segment::id(seg_cnt + db::segment::id(1));
                SegmentFileFactory<T_Meta> sff{ctx, seg_cnt, seg_root};

                return ColSegments<T_Meta>{std::move(sff), segments};
            }
        };

        template<typename T_Meta>
        class Segments {
        private:
            using T_Table = typename T_Meta::Table;
            using hash_algh = typename T_Meta::hash_algh;
            const db::Directory m_root;
            ColSegments<T_Meta> m_segments;
        public:
            explicit Segments(Context<hash_algh> &ctx) :
                    m_root(db::vfs::mkdir(ctx.root.cdx(T_Table::table_name()))),
                    m_segments{ColSegments<T_Meta>::apply(ctx, m_root)} {
//                db::assert_is_context<T_Table>();
            }

            Segments(Segments<T_Meta> &&o) :
                    m_root(o.m_root),
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