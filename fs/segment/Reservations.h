//
// Created by spooky on 2016-04-03.
//

#ifndef PROJECT_RESERVATIONS_H
#define PROJECT_RESERVATIONS_H

#include "../../shared/entities.h"
#include "../../shared/fs.h"
#include "Segment.h"
#include <utility>
#include "ReservationSet.h"

namespace db {
    namespace fs {

        template<typename T_Meta>
        class Reservations {
        private:
            using T_Table = typename T_Meta::Table;
            Segment <T_Meta> m_segment;
            db::ReservationSet<T_Meta::lines()> m_reservations;
        public:

            explicit Reservations(Segment <T_Meta> &&seg) :
                    m_segment{std::move(seg)},
                    m_reservations{m_segment.present_set()}{
            }

            Reservations(Reservations<T_Meta> &&o) :
                    m_segment{std::move(o.m_segment)},
                    m_reservations{std::move(o.m_reservations)} {
//                db::assert_is_meta<T_Meta>();
            }


            Reservations(const Reservations &) = delete;

            Reservation reserve();

            void unreserve(const Reservation &);
        };

        template<typename T_Meta>
        Reservation Reservations<T_Meta>::reserve() {
            return {1l};
        }
        template<typename T_Meta>
        void Reservations<T_Meta>::unreserve(const Reservation &r){
        }

    }
}

#endif //PROJECT_RESERVATIONS_H
