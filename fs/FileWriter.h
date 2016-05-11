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
            const int m_fd;
        public:
            FileWriter(const File &file) : m_file{file}, m_fd{open(file.path.c_str(), O_RDWR | O_CREAT,
                                                                   S_IRUSR | S_IWUSR | S_IRGRP)} {
                if (m_fd == -1) {
                    throw std::runtime_error("FileWriter");
                }
            }

            ~FileWriter() {
                if (m_fd != -1) {
                    ::close(m_fd);
                }
            }

//            int file_descriptor() const {
//                return m_fd;
//            }

            /**
             * pwrite():
             * On success, the number of bytes written is returned (zero indicates that nothing was written), or -1 on error, in which case errno is set to indicate the error.
             *
             * Note:
             * that there's no requirement for pwrite() to write the number of bytes you asked it to.
             * It can write less, and this is not an error. Usually, you'd call pwrite() in a loop -
             *      if it doesn't write all the data, or if it fails with errno==EINTR, then you call it again to write the rest of the data.
             */
            template<size_t size>
            void write(const Buffer<size> &buf, size_t position) {
                size_t remaining = buf.capacity();
                while (remaining > 0) {
                    ssize_t ret = ::pwrite(m_fd, buf.data(), remaining, position);
                    if (ret < 0) {
                        throw std::runtime_error("pwrite");
                    }
                    position = position + ret;
                    remaining = remaining - ret;
                }
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
             *     The call blocks  until  the  device reports that the transfer has completed.
             *     It also flushes metadata information associated with the file.
             */
            void flush_fd() {
                auto err = ::fsync(m_fd);
                if (err != 0) {
                    throw std::runtime_error("flush");
                }
            }

            /**
             * Warning
             * None of these operations writes out the file's metadata.
             * Therefore, unless the application is strictly performing __overwrites__ of already-instantiated disk blocks, there are no guarantees that the data will be available after a crash.
             *
             *
             */
            void flush() {
                off64_t offset = 0;
                off64_t nbytes = 0;
                // Conservative flush all applicable(in range) dirty file pages.
                // This is a write-for-data-integrity operation that will ensure that all pages in the specified range which were dirty when sync_file_range() was called are committed to disk.
                int flags = SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE | SYNC_FILE_RANGE_WAIT_AFTER;
                //there is a ARM version of this function called sync_file_range2
                int ret = ::sync_file_range(m_fd, offset, nbytes, flags);
            }

        };

    }
}


#endif //FS_FILESYSTEM_H