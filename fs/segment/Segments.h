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
            sp::List<Segment<T_Table>> m_vector;
        public:
            ColSegments(index_type p_index, const Directory &p_root) : m_seg_counter{p_index},
                                                                       m_root{p_root.cd("segment")} {
                db::vfs::mkdir(m_root);
            }
            Reservation reserve(){
                return {0};
            }
        };

        template<typename T_Table>
        class Segments {
        private:
            const db::Directory m_root;
            ColSegments<T_Table> m_segments;
        public:
            Segments(const Context &ctx) : m_root{ctx.root.cd(T_Table::table_name())},
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
