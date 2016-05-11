//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTS_H
#define PROJECT_SEGMENTS_H

#include "../../config/Configuration.h"
#include <atomic>
#include "../../shared/Assertions.h"

namespace db {
    namespace fs {
        template<typename T_Table>
        class Segments {
        private:
            using index_type = db::segment::index_type;
            std::atomic<index_type> m_seg_counter;
        public:
            Segments(index_type id = 0l) : m_seg_counter{id} {
                db::assert_is_table<T_Table>();
            }

            Segments(const Segments &o) = delete;

            Reservation reserve();
        };
    }
}
#endif //PROJECT_SEGMENTS_H
