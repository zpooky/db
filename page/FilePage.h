#ifndef PROJECT_FILE_PAGE_H
#define PROJECT_FILE_PAGE_H

#include "../fs/shared.h"
#include "../shared/shared.h"
#include "io/FilePageMeta.h"
#include <algorithm>
#include <cassert>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace page {
struct MMAP_File {
  int fd;

  MMAP_File() : fd(-1) {
  }

  explicit MMAP_File(const db::File &file) : MMAP_File() {
    int flags = O_RDWR;
    int permssion = 0;
    fd = ::open(file.c_str(), flags, permssion);
    error("FileWriter", fd);
  }

  ~MMAP_File() {
    if (fd != -1) {
      ::close(fd);
      fd = -1;
    }
  }
};
struct MMAP_Buffer {
  uint8_t *buffer;
  MMAP_Buffer() : buffer(nullptr) {
  }
  explicit MMAP_Buffer(const MMAP_File &file) : MMAP_Buffer() {
    int mmap_flags = PROT_READ | PROT_WRITE;
    auto ptr = mmap(nullptr, 4096, mmap_flags, MAP_SHARED, file.fd, 0);
    // TODO error
    buffer = static_cast<uint8_t *>(ptr);
  }
  ~MMAP_Buffer() {
    if (buffer) {
      // TODO  munmap(buffer, 4096);
      buffer = nullptr;
    }
  }
};
template <typename Meta_t>
class FilePage {
private:
  using Page_t = typename Meta_t::Page;
  using Table_t = typename Meta_t::Table;
  using version_t = db::raw::version_t;
  using id_t = db::raw::id;

private:
  FilePageMeta m_meta;
  MMAP_File m_file;
  MMAP_Buffer m_buffer;

public:
  explicit FilePage(FilePageMeta &&meta)
      : m_meta{std::move(meta)}, m_file(m_meta.file), m_buffer() {
    m_buffer = MMAP_Buffer(m_file);
  }

  FilePage(const FilePage &) = delete;

  FilePage(FilePage &&o) : m_meta{std::move(o.m_meta)}, m_file(), m_buffer() {
    std::swap(m_file, o.m_file);
    std::swap(m_buffer, o.m_buffer);
  }

public:
  auto id() const {
    return m_meta.id;
  }
  id_t create(page::position pos, const Table_t &data) {
    assert(pos < m_meta.number);
    // assert(Table_t::version == m_meta.version);
    return db::raw::START_ID;
  }
};
}

#endif
