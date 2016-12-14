//
// Created by spooky on 2016-03-07.
//

#ifndef FS_FILESYSTEM_H
#define FS_FILESYSTEM_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "../shared/Buffer.h"
#include "../shared/entities.h"
#include "Line.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

// extern int errno;
namespace {
static void error(const char *fun, int ret);
void error(const char *func, int ret) {
  if (ret == -1) {
    int errnum = errno;
    std::string err(func);
    err.append("(");
    // err.append(m_file.path);
    err.append("): ");
    err.append(strerror(errnum));
    throw std::runtime_error(err);
  }
}
}
namespace db {
class FileWriter {
private:
  File m_file;
  int m_fd;

public:
  explicit FileWriter(const File &file) : m_file(file) {
    int flags = O_RDWR | O_CREAT;
    int permssion = S_IRUSR | S_IWUSR | S_IRGRP;
    m_fd = ::open(file.path.c_str(), flags, permssion);
    ::error("FileWriter", m_fd);
  }

  FileWriter(const FileWriter &) = delete;

  FileWriter(FileWriter &&o) : m_file(o.m_file), m_fd{o.m_fd} {
    o.m_fd = -1;
  }

  ~FileWriter() {
    if (m_fd != -1) {
      //                    flush();
      ::close(m_fd);
      m_fd = -1;
    }
  }

  //            int file_descriptor() const {
  //                return m_fd;
  //            }

  /**
   * pwrite():
   * On success, the number of bytes written is returned (zero indicates that
   * nothing was written), or -1 on error, in which case errno is set to
   * indicate the error.
   *
   * Note:
   * that there's no requirement for pwrite() to write the number of bytes you
   * asked it to.
   * It can write less, and this is not an error. Usually, you'd call pwrite()
   * in a loop -
   *  if it doesn't write all the data, or if it fails with errno==EINTR,
   * then you call it again to write the rest of the data.
   */
  template <size_t size>
  void write(const Buffer<size> &buf, size_t position) {
    size_t remaining = buf.size();
    while (remaining > 0) {
      ssize_t ret = ::pwrite(m_fd, buf.full_data(), remaining, position);
      ::error("write", ret);
      position = position + ret;
      remaining = remaining - ret;
    }
  }

  template <size_t t_size>
  void init_write(const std::array<char, t_size> &buffer, size_t cnt) {
    int ret = ::write(m_fd, buffer.begin(), cnt);
    ::error("init_write", ret);
  }

  /*
   * fsync()
   * transfers (flushes) all modified in-core data of (i.e., modified buffer
   * cache pages for)
   * the file referred to by the file descriptor fd to the disk device (or other
   * permanent storage device)
   * so that all changed information can be retrieved even after the system
   * crashed or was rebooted.
   *
   * - Flushes data
   * - Flushes metadata information associated with the file. Resize, make file
   *
   * This includes writing through or flushing a disk cache if present.
   * The call blocks  until  the  device reports that the transfer has
   * completed.
   */
  void flush_fd() {
    auto ret = ::fsync(m_fd);
    ::error("flush_fd", ret);
  }

  void data_flush() {
    auto ret = ::fdatasync(m_fd);
    ::error("data_flush", ret);
  }

  /**
   * Warning
   * None of these operations writes out the file's metadata.
   * Therefore, unless the application is strictly performing __overwrites__ of
   * already-instantiated disk blocks, there are no guarantees that the data
   * will be available after a crash.
   *
   *
   */
  void flush() {
    off64_t offset = 0;
    off64_t nbytes = 0;
    // Conservative flush all applicable(in range) dirty file pages.
    // This is a write-for-data-integrity operation that will ensure that all
    // pages in the specified range which were dirty when sync_file_range() was
    // called are committed to disk.
    int flags = SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE |
                SYNC_FILE_RANGE_WAIT_AFTER;
    // there is a ARM version of this function called sync_file_range2
    int ret = ::sync_file_range(m_fd, offset, nbytes, flags);
    ::error("flush", ret);
  }

  const File &file() const {
    return m_file;
  }

private:
};

struct AlignedBuffer {
private:
  uint8_t *m_alloc;
  uint8_t *m_aligned;

  size_t m_index = 0;
  size_t m_size = 0;
  size_t m_capacity;

public:
  /**
   * /malloc/ gives you a chunk of memory that could have any alignment.
   * /posix_memalign/ gives you a chunk of memory that is guaranteed
   * to have the requested alignment.
   *
   * So the result of posix_memalign(&p, 32, 128) will be a 128-byte
   * chunk of memory whose start address is guaranteed to be a multiple of 32.
   */
  explicit AlignedBuffer(size_t alignment)
      : AlignedBuffer(alignment, alignment) {
  }
  explicit AlignedBuffer(size_t alignment, size_t bytes)
      : m_capacity(size_t(sizeof(uint8_t)) * bytes) {
    assert(alignment > 0);
    assert(bytes > 0);
    assert(bytes <= alignment);
    assert(alignment % bytes == 0);
    /* Alloc dubble the size to ensure alignment fits
     */
    size_t alloc_size(2 * bytes);
    m_alloc = (uint8_t *)::calloc(alloc_size, sizeof(uint8_t));
    if (!m_alloc) {
      throw std::runtime_error("AlignedBuffer alloc");
    }
    /* Number offset bytes
     */
    size_t diff = ((size_t)m_alloc) % alignment;
    assert(alloc_size - diff >= bytes);
    /* Create a aligned pointer
     */
    m_aligned = m_alloc + diff;
    //::posix_memalign();
  }
  void flip() {
    std::swap(m_index, m_size);
  }
  void clear() {
    // TODO zero buffer
    m_index = m_size = 0;
  }
  size_t index() const {
    return m_index;
  }
  size_t size() const {
    return m_size;
  }
  size_t capacity() const {
    return m_capacity;
  }

  bool is_full() const {
    assert(size() <= capacity());
    return size() == capacity();
  }
  bool is_empty() const {
    return m_index == 0;
  }

  const uint8_t *full_data() const {
    return m_aligned;
  }

  ~AlignedBuffer() {
    if (m_alloc) {
      ::free(m_alloc);
      m_alloc = nullptr;
    }
  }
};

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
class DirectFileWriter {
private:
  int m_fd;

public:
  explicit DirectFileWriter(const File &file) {
    int flags = O_WRONLY | O_CREAT | O_DIRECT;
    int permssion = S_IRUSR | S_IWUSR | S_IRGRP;
    m_fd = open(file.c_str(), flags, permssion);
    ::error("DFW", m_fd);
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
      ::error("write", ret);
      remaining = remaining - ret;
      assert(remaining != 0);
    }
  }

  void flush() {
    int ret = ::fsync(m_fd);
    ::error("DFW flush", ret);
  }
};
}

#endif // FS_FILESYSTEM_H
