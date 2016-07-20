//
// Created by spooky on 2016-03-06.
//

#ifndef FS_RESERVATION_H
#define FS_RESERVATION_H

#include "../shared/fs.h"
#include "Segment.h"

namespace db {
    namespace fs {
        /**
         * Thread safe
         * individual segment "Line reservation manager"
         *
         * Segment
         * |======.---------.
         * |Sector|   Line   |<---| Reserve
         * |      |----------|<---|
         * |      |   Line   |<---|
         * |======|----------|
         * |Sector|   Line   |
         * |      |----------|
         * |      |   Line   |
         * |======|----------|
         */
        template<typename t_Table>
        class SegmentReservations {
        private:
            const Segment<t_Table> m_segment;
        public:
            explicit SegmentReservations(const Segment<t_Table> &s) : m_segment{s} {
                db::assert_is_table<t_Table>();
            }

            Reservation reserve();

            size_t free_count() {
                return 0;
            }
        };

        template<typename t_Table>
        Reservation SegmentReservations<t_Table>::reserve() {
            return {1l};
        }
    }
}

#endif //PROJECT_RESERVATION_H
