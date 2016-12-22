#ifndef _PROJECT_DIRECT_FILE_WRITER_H
#define _PROJECT_DIRECT_FILE_WRITER_H

/**
 * ## O_DIRECT
 * Direct I/O is a feature of the file system whereby file reads and
 * writes go directly from the applications to the storage device, bypassing the
 * operating system read and write caches.
 * All I/O operations must be done in block-size multiples of 512 bytes.
 * The memory being read from or written to must also be 512-byte aligned.
 *
 * ### Advantages
 * Data is transferred directly from the disk to the application buffer,
 * without the use of the kernel file buffer cache.
 *
 * ### Disadvantages
 * Direct I/O bypasses filesystem read-ahead
 *
 * ### Note
 * The alignment of user buffer and file offset must all be multiples of the
 * logical block size.
 * Under Linux 2.6, alignment to 512-byte boundaries suffices.
 */
#include "../shared/entities.h"
#include "shared.h"
#include "AlignedBuffer.h"
#include <cstring>
#include <fcntl.h>

namespace db {

class DirectFileWriter {
private:
  int m_fd;

public:
  explicit DirectFileWriter(const File &file) {
    int flags = O_WRONLY | O_CREAT | O_DIRECT;
    int permssion = S_IRUSR | S_IWUSR | S_IRGRP;
    m_fd = open(file.c_str(), flags, permssion);
    error("DFW", m_fd);
  }
  ~DirectFileWriter() {
    if (m_fd != -1) {
      ::close(m_fd);
      m_fd = -1;
    }
  }
  void write(const AlignedBuffer &buff) {
    size_t remaining = buff.size();
    while (remaining > 0) {
      ssize_t ret = ::write(m_fd, buff.full_data(), remaining);
      error("write", ret);
      remaining = remaining - ret;
      assert(remaining != 0);
    }
  }

  void flush() {
    int ret = ::fsync(m_fd);
    error("DFW flush", ret);
  }
};
}
#endif
