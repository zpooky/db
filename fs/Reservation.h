//
// Created by spooky on 2016-03-06.
//

#ifndef PROJECT_RESERVATION_H
#define PROJECT_RESERVATION_H

#include "../shared/fs.h"

namespace db {
    namespace fs {
        class Reservations {
        public:
            Reservation reserve();
        };
    }
}

#endif //PROJECT_RESERVATION_H
