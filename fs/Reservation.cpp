//
// Created by spooky on 2016-03-06.
//

#include "Reservation.h"
#include "../shared/fs.h"

namespace db {
    namespace fs {
        Reservation Reservations::reserve() {
            return {1l};
        }
    }
}