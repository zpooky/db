//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_RESERVATIONSET_H
#define PROJECT_RESERVATIONSET_H

#include <bitset>
#include <utility>
#include "PresentSet.h"

namespace db {
    template<size_t T_Size>
    class ReservationSet {
    private:
        std::bitset<T_Size> m_b;
    public:
        template<typename t_Meta>
        explicit ReservationSet(const PresentSet<t_Meta> &p) {
        }

        ReservationSet(const ReservationSet &) = delete;

        ReservationSet(ReservationSet &&o) :
                m_b{std::move(m_b)} {
        }

    };
}

#endif //PROJECT_RESERVATIONSET_H
