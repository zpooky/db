//
// Created by spooky on 2016-04-23.
//

#ifndef PROJECT_BITSET_H
#define PROJECT_BITSET_H

#include <bitset>

namespace sp {
    template<size_t T_Size>
    class CBitset {
    private:

    public:
        CBitset() {

        }

        explicit CBitset(const std::bitset<T_Size> &init) {

        }
    };
}


#endif //PROJECT_BITSET_H
