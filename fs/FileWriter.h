//
// Created by spooky on 2016-03-07.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H

#include <unistd.h>
#include <fcntl.h>
#include "Line.h"
#include "../shared/Buffer.h"
#include "../shared/entities.h"
#include <stdexcept>

namespace db {
    namespace fs {
        class FileWriter {
        private:
            const File m_file;
            int m_fd;
        public:
            FileWriter(const File &file) : m_file{file} {
                m_fd = open(file.path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
                if (m_fd == -1) {
                    throw std::runtime_error("FileWriter");
                }
            }

            ~FileWriter() {
                if (m_fd != -1) {
                    close(m_fd);
                    m_fd = -1;
                }
            }

            int file_descriptor() const {
                return m_fd;
            }

            template<size_t size>
            bool write(const Reservation &r, const Line<size> &l) {
                auto buf = buffer(l);
                ::pwrite(m_fd, buf.data(), buf.capacity(), r.position);
                return true;
            }

            template<size_t t_size>
            void init_write(const std::array<char, t_size> &buffer, size_t cnt) {
                ::write(m_fd, buffer.begin(), cnt);
            }

            /*
             * fsync() transfers ("flushes") all modified in-core data of (i.e., modified buffer cache pages for)
             *     the file referred to by the file descriptor fd to the disk device (or other permanent storage device)
             *     so that all changed information can be retrieved even after the system crashed or was rebooted.
             *     This includes writing through or flushing a disk cache if present.
             *     The call blocks  until  the  device
             *     reports that the transfer has completed.
             *     It also flushes metadata information associated with the file (see stat(2)).
             */
            void flush() {
                auto err = fsync(m_fd);
                if (err != 0) {
                    throw std::runtime_error("flush");
                }
            }

        };

    }
}


#endif //FS_FILESYSTEM_H
