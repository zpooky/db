//
// Created by spooky on 2016-03-07.
//

#include "Filesystem.h"
#include "../shared/fs.h"

namespace db {
    namespace fs {
        bool Filesystem::write(const Reservation &) {
            return true;
        }
    }
}