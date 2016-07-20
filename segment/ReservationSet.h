//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_RESERVATIONSET_H
#define PROJECT_RESERVATIONSET_H

#include <bitset>
#include <utility>
#include "PresentSet.h"
#include "../shared/Maybe.h"

namespace db {
    template<size_t T_Size>
    class ReservationSet {
    private:
        sp::CBitset<T_Size> m_bitset;
        std::atomic<size_t> m_cnt;
    public:
        template<typename t_Meta>
        explicit ReservationSet(const PresentSet<t_Meta> &p)
                : m_bitset{p.get_bitset()},
                  m_cnt{m_bitset.find_first(false)} {
        }

        ReservationSet(const ReservationSet &) = delete;

        ReservationSet(ReservationSet &&o) :
                m_bitset{std::move(o.m_bitset)},
                m_cnt{o.m_cnt.load()} {
        }

        sp::Maybe<db::reservation::id> reserve() {
            using Return_t = sp::Maybe<db::reservation::id>;
            size_t reserved = m_bitset.swap_first(m_cnt, true);
            m_cnt.store(reserved);
            if (reserved != m_bitset.npos) {
                return Return_t{reserved};
            }
            return Return_t{};
        }


        bool has_free() const {
            bool result = !m_bitset.all(m_cnt.load(), true);
            return result;
        }
    };
}

#endif //PROJECT_RESERVATIONSET_H
