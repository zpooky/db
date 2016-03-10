//
// Created by spooky on 2016-03-07.
//

#ifndef PROJECT_FILESYSTEM_H
#define PROJECT_FILESYSTEM_H

#include <unistd.h>
#include "Reservation.h"
#include "Line.h"
#include "Buffer.h"

namespace db {
    namespace fs {
        class File {
        private:
            const int m_fd;
        public:
            File(int fd) : m_fd(fd) {

            }

            template<size_t size>
            bool write(const Reservation &r, const Line<size> &l) {
                auto buf = buffer(l);
                ::pwrite(m_fd, buf.data(), buf.length(), r.position);
                return true;
            }
        };

    }
}


#endif //PROJECT_FILESYSTEM_H
