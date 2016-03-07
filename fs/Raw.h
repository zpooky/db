//
// Created by spooky on 2016-03-03.
//

#ifndef DB_RAW_H
#define DB_RAW_H

#include <stddef.h>
#include "../shared/fs.h"

namespace db {
    namespace fs {
        template<size_t BYTES>
        struct Raw {
            db::rid id;
            db::crc32 checksum;
            db::State state;
            db::raw data[BYTES];
        };
    }
}

#endif //DB_RAW_H
