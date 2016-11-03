//
// Created by spooky on 2016-03-03.
//

#ifndef DB_FS_H
#define DB_FS_H

#include <stddef.h>
#include <array>
#include <string>
#include <utility>
#include "../config/Configuration.h"

namespace db {

//    template <typename Table_t>
    struct Reservation {
        const size_t position;
        const db::segment::id segment;

        explicit Reservation() :
                Reservation(0, 0) {
            //TODO fix this
        }

        explicit Reservation(size_t p_position, db::segment::id p_segment) :
                position{p_position},
                segment{p_segment} {
        }
    };

    class Table {
//        static size_t size() const noexcept = 0;
    };

//Raw
    using rid = uint64_t;
    constexpr rid EMPTY_LINE = 0ul;
    template<size_t size>
    using raw = std::array<uint8_t, size>;
    enum class RState : uint8_t {
        RESERVED = 0, PREPARED, ROLLEDBACK, COMMITED
    };
//Journal

    //

}
namespace journal {

    using journal_id = uint64_t;
}

#endif //DB_FS_H
