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
        sp::CBitset<T_Size> m_bitset;
    public:
        template<typename t_Meta>
        explicit ReservationSet(const PresentSet<t_Meta> &p) {
        }

        ReservationSet(const ReservationSet &) = delete;

        ReservationSet(ReservationSet &&o) :
                m_bitset{std::move(o.m_bitset)} {
        }


        bool has_free() const {
            return false;
        }
    };
}

#endif //PROJECT_RESERVATIONSET_H
