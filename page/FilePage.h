#ifndef PROJECT_FILE_PAGE_H
#define PROJECT_FILE_PAGE_H

#include "../fs/Line.h"
#include "../fs/shared.h"
#include "../shared/LittleEndian.h"
#include "../shared/Maybe.h"
#include "../shared/shared.h"
#include "io/FilePageMeta.h"
#include <algorithm>
#include <cassert>
#include <fcntl.h>
#include <iostream>
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

  MMAP_File(const MMAP_File &) = delete;

  MMAP_File(MMAP_File &&o) : fd{-1} {
    std::swap(fd, o.fd);
  }
  MMAP_File(const MMAP_File &&) = delete;

  explicit MMAP_File(const db::File &file) : MMAP_File() {
    int flags = O_RDWR;
    int permssion = 0;
    fd = ::open(file.c_str(), flags, permssion);
    error("MMAP_File", fd);
  }

  MMAP_File &operator=(MMAP_File &) = delete;
  MMAP_File &operator=(MMAP_File &&o) {
    std::swap(fd, o.fd);
    return *this;
  }

  ~MMAP_File() {
    release();
  }

private:
  void release() {
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

  MMAP_Buffer(const MMAP_Buffer &) = delete;

  MMAP_Buffer(MMAP_Buffer &&o) : buffer(nullptr), m_size(0) {
    std::swap(buffer, o.buffer);
    std::swap(m_size, o.m_size);
  }

  MMAP_Buffer &operator=(MMAP_Buffer &) = delete;
  MMAP_Buffer &operator=(MMAP_Buffer &&o) {
    std::swap(buffer, o.buffer);
    std::swap(m_size, o.m_size);
    return *this;
  }

  explicit MMAP_Buffer(const MMAP_File &file, size_t size)
      : buffer(nullptr), m_size(size) {
    int prot = PROT_READ | PROT_WRITE;
    auto ptr = ::mmap(0, m_size, prot, MAP_SHARED, file.fd, 0);
    error_mmap("MMAP_Buffer", ptr);
    buffer = reinterpret_cast<uint8_t *>(ptr);
  }
  auto at_offset(size_t offset) const {
    return buffer + offset;
  }
  auto size() const {
    return m_size;
  }

  ~MMAP_Buffer() {
    release();
  }

private:
  void release() {
    if (buffer) {
      ::munmap(buffer, m_size);
      buffer = nullptr;
      m_size = 0;
    }
  }
};

template <typename Meta_t>
class MMapFilePage {
private:
  using Table_t = typename Meta_t::latest;
  using hash_t = typename Meta_t::hash_t;

private:
  MMAP_File m_file;
  MMAP_Buffer m_buffer;

public:
  explicit MMapFilePage(const FilePageMeta &meta)
      : m_file(meta.file), m_buffer(m_file, meta.line_size * meta.lines) {
    // TODO headers and stuff
  }

  MMapFilePage(MMapFilePage &&o)
      : m_file(std::move(o.m_file)), m_buffer(std::move(o.m_buffer)) {
  }

  MMapFilePage(const MMapFilePage &&) = delete;

  MMapFilePage &operator=(const MMapFilePage &) = delete;
  MMapFilePage &operator=(const MMapFilePage &&) = delete;

  void write(page::position p, const db::Line<Table_t, hash_t> &l) {
    // TODO header
    size_t offset = l.size() * p;
    assert(offset + l.size() <= m_buffer.size());

    auto ptr = m_buffer.at_offset(offset);
    db::PointerBuffer buffer(ptr, l.size());
    // TODO endianess
    using Endianess = db::LittleEndian;
    l.template write<Endianess, db::PointerBuffer>(buffer);
  }
};

template <typename Meta_t>
class FilePage {
private:
  using Page_t = typename Meta_t::Page;
  using Table_t = typename Meta_t::latest;
  using hash_t = typename Meta_t::hash_t;
  using Line_t = db::Line<Table_t, hash_t>;
  using version_t = db::raw::version_t;
  using id_t = db::raw::id;

private:
  FilePageMeta m_meta;
  MMapFilePage<Meta_t> m_mapped;

public:
  explicit FilePage(const FilePageMeta &meta) : m_meta{meta}, m_mapped{m_meta} {
  }

  FilePage(const FilePage &) = delete;

  FilePage(FilePage &&o)
      : m_meta{std::move(o.m_meta)}, m_mapped{std::move(o.m_mapped)} {
  }

  FilePage &operator=(const FilePage &) = delete;
  FilePage &operator=(const FilePage &&) = delete;

public:
  auto id() const {
    return m_meta.id;
  }

  id_t create(db::raw::id id, page::position pos, const Table_t &data) {
    assert(pos < m_meta.lines);
    // assert(Table_t::version == m_meta.version);
    auto version = db::raw::START_VERSION;
    Line_t line(db::LineMeta(id, version), data);
    m_mapped.write(pos, line);
    return db::raw::START_ID;
  }

  void remove(page::position pos) {
    assert(pos < m_meta.lines);
    // m_mapped.write(Line_t());
  }

  sp::Maybe<Table_t> read(page::position pos) {
    assert(pos < m_meta.lines);
  }
};
}

#endif
