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

        template<typename t_Table>
        class Reservations {
        private:
            Segment<t_Table> m_segment;
        public:

            Reservations(Reservations<t_Table> &&o) : m_segment{std::move(o.m_segment)} {
            }

            explicit Reservations(Segment<t_Table> &&seg) : m_segment{std::move(seg)} {
            }

//            Reservations(const Reservations &o) = delete;

            Reservation reserve();
        };

        template<typename t_Table>
        Reservation Reservations<t_Table>::reserve() {
            return {1l};
        }

    }
}

#endif //PROJECT_RESERVATIONS_H
