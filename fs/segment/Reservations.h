//
// Created by spooky on 2016-04-03.
//

#ifndef PROJECT_RESERVATIONS_H
#define PROJECT_RESERVATIONS_H

#include "../../shared/entities.h"
#include "../../shared/fs.h"
#include "Segment.h"
#include <utility>
#include <bitset>

namespace db {
    namespace fs {
        template<size_t t_Size>
        class ReservationSet {
        private:
            std::bitset<t_Size> m_b;
        public:
        };

        template<typename T_Meta>
        class Reservations {
        private:
            using T_Table = typename T_Meta::Table;
            Segment <T_Meta> m_segment;
            ReservationSet<T_Meta::lines()> m_reservations;
        public:

            Reservations(Reservations<T_Meta> &&o) :
                    m_segment{std::move(o.m_segment)},
                    m_reservations{std::move(o.m_reservations)} {
//                db::assert_is_meta<T_Meta>();
            }

            explicit Reservations(Segment <T_Meta> &&seg) :
                    m_segment{std::move(seg)} {
            }

//            Reservations(const Reservations &o) = delete;

            Reservation reserve();
        };

        template<typename T_Meta>
        Reservation Reservations<T_Meta>::reserve() {
            return {1l};
        }

    }
}

#endif //PROJECT_RESERVATIONS_H
