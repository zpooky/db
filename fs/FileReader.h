//
// Created by spooky on 2016-07-06.
//

#ifndef PROJECT_FILEREADER_H
#define PROJECT_FILEREADER_H

#include "../shared/Buffer.h"
#include "../shared/entities.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

namespace db {
namespace fs {
class FileReader {
private:
  File m_file;
  int m_fd;

public:
  explicit FileReader(const File &file)
      : m_file(file), m_fd{open(file.path.c_str(), O_RDONLY)} {
    error("FileReader", m_fd);
  }

  FileReader(FileReader &&o) : m_file(o.m_file), m_fd(o.m_fd) {

    o.m_fd = -1;
  }

  FileReader(const FileReader &) = delete;

  ~FileReader() {
    if (m_fd != -1) {
      ::close(m_fd);
      m_fd = -1;
    }
  }

private:
  void error(const char *func, int ret) const {
    if (ret == -1) {
      int errnum = errno;
      std::string err(func);
      err.append("(");
      err.append(m_file.path);
      err.append("): ");
      err.append(strerror(errnum));
      throw std::runtime_error(err);
    }
  }

public:
  void read(db::BaseBuffer &buffer);
};

void FileReader::read(db::BaseBuffer &buffer) {
  size_t length(buffer.capacity() - buffer.position());
  if (length > size_t(0)) {
    ssize_t read = ::read(m_fd, buffer.writable_data(), length);
    error("read", read);
    buffer.position(buffer.position() + read);
  }
}
}
}

#endif // PROJECT_FILEREADER_H
