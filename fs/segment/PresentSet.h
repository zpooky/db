//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_PRESENTSET_H
#define PROJECT_PRESENTSET_H

#include <bitset>
#include "../../shared/CBitset.h"
#include "../Line.h"

namespace db {

    template<typename T_Meta>
    class PresentSet {
    private:
        sp::CBitset<db::fs::Line_size<T_Meta>::value()> m_bitset;
    public:
        PresentSet() :
                m_bitset{} {

        }

        explicit PresentSet(const std::bitset<db::fs::Line_size<T_Meta>::value()> &init) :
                m_bitset{init} {

        }

        PresentSet(const PresentSet &) = delete;

        PresentSet(PresentSet &&o) :
                m_bitset(std::move(o.m_bitset)) {

        }

    private:
    };

}
#endif //PROJECT_PRESENTSET_H
