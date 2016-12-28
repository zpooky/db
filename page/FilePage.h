#ifndef PROJECT_FILE_PAGE_H
#define PROJECT_FILE_PAGE_H

#include "../fs/Line.h"
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
class MMAP_File {
  public:
  int fd;

  MMAP_File() : fd(-1) {
  }

  explicit MMAP_File(const db::File &file) : MMAP_File() {
    int flags = O_RDWR;
    int permssion = 0;
    fd = ::open(file.c_str(), flags, permssion);
    error("MMAP_File", fd);
  }

  ~MMAP_File() {
    if (fd != -1) {
      ::close(fd);
      fd = -1;
    }
  }
};

class MMAP_Buffer {
  public:
  uint8_t *buffer;
  size_t m_size;

  MMAP_Buffer() : buffer(nullptr), m_size(0) {
  }

  explicit MMAP_Buffer(const MMAP_File &file, size_t size)
      : buffer(nullptr), m_size(size) {
    int mmap_flags = PROT_READ | PROT_WRITE;
    auto ptr = ::mmap(nullptr, m_size, mmap_flags, MAP_SHARED, file.fd, 0);
    error_mmap("MMAP_Buffer", ptr);
    buffer = static_cast<uint8_t *>(ptr);
  }
  uint8_t *at_offset(size_t offset) const {
    return &buffer[offset];
  }

  ~MMAP_Buffer() {
    if (buffer) {
      ::munmap(buffer, m_size);
      buffer = nullptr;
    }
  }
};

template <typename Meta_t>
class MMapFilePage {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;

private:
  const size_t m_line_size;
  MMAP_File m_file;
  MMAP_Buffer m_buffer;

public:
  explicit MMapFilePage(const FilePageMeta &meta)
      : m_line_size(meta.line_size), m_file(meta.file), m_buffer() {
    // TODO headers and stuff
    size_t size = m_line_size * meta.number;
    m_buffer = MMAP_Buffer(m_file, size);
  }
  MMapFilePage(MMapFilePage &&o)
      : m_line_size(o.m_line_size), m_file(), m_buffer() {
    std::swap(m_file, o.m_file);
    std::swap(m_buffer, o.m_buffer);
  }
  void write(page::position p, const db::Line<Table_t, hash_t> &l) {
    // TODO header
    size_t offset = m_line_size * p;
    auto ptr = m_buffer.at_offset(offset);
    l.write(ptr);
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
  MMapFilePage<Meta_t> m_mapped;

public:
  explicit FilePage(FilePageMeta &&meta)
      : m_meta{std::move(meta)}, m_mapped{m_meta} {
  }

  FilePage(const FilePage &) = delete;

  FilePage(FilePage &&o)
      : m_meta{std::move(o.m_meta)}, m_mapped{std::move(o.m_mapped)} {
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
