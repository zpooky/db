//
// Created by spooky on 2016-03-03.
//

#ifndef DB_FS_H
#define DB_FS_H

#include <stddef.h>
#include <array>
#include <string>
#include <utility>

namespace db {
    using segment_id = size_t;

//    template <typename Table_t>
    struct Reservation {
        const size_t position;
        const segment_id segment;

        explicit Reservation(size_t p_position, segment_id p_segment) :
                position{p_position},
                segment{p_segment} {
        }
    };

    class Table {
//        static size_t size() const noexcept = 0;
    };

    using TableVersion_t = uint16_t;
//Raw
    using rid = uint64_t;
    constexpr rid EMPTY_LINE = 0ul;
    template<size_t size>
    using raw = std::array<uint8_t, size>;
    enum class RState : uint8_t {
        RESERVED = 0, PREPARED, ROLLEDBACK, COMMITED
    };
//Journal
    using journal_id = uint64_t;

    //

}


#endif //DB_FS_H
