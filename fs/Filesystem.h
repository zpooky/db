//
// Created by spooky on 2016-03-07.
//

#ifndef PROJECT_FILESYSTEM_H
#define PROJECT_FILESYSTEM_H

#include "Reservation.h"

namespace db {
    namespace fs {
        class Filesystem {
        public:
            static bool write(const Reservation&);
        };
    }
}


#endif //PROJECT_FILESYSTEM_H
