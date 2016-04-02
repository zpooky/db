//
// Created by spooky on 2016-03-07.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H

#include <unistd.h>
#include <fcntl.h>
#include "Line.h"
#include "../shared/Buffer.h"

namespace db {
    namespace fs {
        class File {
        private:
            const std::string m_file;
            int m_fd;
        public:
            File(const std::string &file) : m_file{file}{
                m_fd = open(file.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
                if (m_fd == -1) {
                }
            }

            ~File() {
                if (m_fd != -1) {
                    close(m_fd);
                    m_fd = -1;
                }
            }

            template<size_t size>
            bool write(const Reservation &r, const Line<size> &l) {
                auto buf = buffer(l);
                ::pwrite(m_fd, buf.data(), buf.capacity(), r.position);
                return true;
            }
        };

    }
}


#endif //FS_FILESYSTEM_H
