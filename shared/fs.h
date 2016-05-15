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
    struct Reservation {
        const size_t position;

        Reservation(size_t p_position) : position{p_position} {
        }
    };

    class Table {
//        static size_t size() const noexcept = 0;
    };
//Raw
    using rid = unsigned long;
//    using crc32 = std::array<unsigned char, 4>;
    template<size_t size>
    using raw = std::array<unsigned char, size>;
    enum class RState : unsigned char {
        RESERVED = 0, PREPARED, ROLLEDBACK, COMMITED
    };
//Journal
    using journal_id = unsigned long;

    //

}


#endif //DB_FS_H
