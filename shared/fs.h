//
// Created by spooky on 2016-03-03.
//

#ifndef DB_FS_H
#define DB_FS_H

#include <stddef.h>

namespace db {
    class Table {
        virtual size_t size() const noexcept = 0;
    };

    struct Reservation {

    };
//Raw
    using rid = unsigned long;
    using crc32 = unsigned char[4];
    using raw = unsigned char;
    enum class State : unsigned char {
        RESERVED = 0, PREPARED, ROLLEDBACK, COMMITED
    };
//Journal
    using journal_id = unsigned long;
}


#endif //DB_FS_H
