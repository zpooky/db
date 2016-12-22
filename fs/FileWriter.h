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
#include "shared.h"
#include "Line.h"
#include <cassert>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

// extern int errno;
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
    error("FileWriter", m_fd);
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
      error("write", ret);
      position = position + ret;
      remaining = remaining - ret;
    }
  }

  template <size_t t_size>
  void init_write(const std::array<char, t_size> &buffer, size_t cnt) {
    int ret = ::write(m_fd, buffer.begin(), cnt);
    error("init_write", ret);
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
    error("flush_fd", ret);
  }

  void data_flush() {
    auto ret = ::fdatasync(m_fd);
    error("data_flush", ret);
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
    error("flush", ret);
  }

  const File &file() const {
    return m_file;
  }

private:
};

}
#endif // FS_FILESYSTEM_H
