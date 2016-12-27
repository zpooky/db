//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_FORMAT_H
#define PROJECT_FORMAT_H

#include "../../fs/FileReader.h"
#include "../../fs/FileWriter.h"
#include "../../fs/Line.h"
#include "../../shared/conversions.h"
#include "../../shared/vfs.h"
#include "../../shared/entities.h"
#include "../io/FilePageMeta.h"
#include "../../segment/PresentSet.h"
/*
 * Header:
 * size in bytes:
 * |byte|unsigned short|db::table::name::length|unsigned short|unsigned short|
 *
 * header fields:
 * .-------------------------------------------------------------------------------.
 * |endianess|db segment format version|Table name|Line size|Table version|
 * `-------------------------------------------------------------------------------`
 *---
 *partition type|partition meta data length|meta data|
 * # partition type
 * none                     0
 * day based partitioning   1
 * # endianess
 * - big endian     0
 * - little endian  1
 */
namespace page {

/**
 * Used to create a segment file according to the v1 format.
 */
template <typename T_Meta>
class V1SegmentInit {
private:
  using Table_t = typename T_Meta::Table;
  const db::Directory m_root;

public:
  explicit V1SegmentInit(const db::Directory &root) : m_root(root) {
    db::assert_is_table<Table_t>();
  }

  FilePageMeta create(db::segment::id);
};

template <typename T_Meta>
FilePageMeta V1SegmentInit<T_Meta>::create(db::segment::id sid) {
  using Table_t = typename T_Meta::Table;
  db::Filename filename{db::Segment_name::name(sid)};

  using capacity = unsigned long long;
  constexpr size_t line_size = db::fs::Line_size<T_Meta>::value();
  constexpr size_t lines = T_Meta::extent_lines();
  capacity target = line_size * lines;
  //
  db::File file = m_root.cd(filename);
  db::FileWriter stream{file};
  // TODO use heap buffer instead
  std::array<char, vfs::page::default_size> buf{0};
  do {
    size_t counter = std::min<size_t>(buf.size(), target);
    target = target - counter;
    stream.init_write(buf, counter);
  } while (target > 0);
  stream.flush();
  vfs::sync(db::Directory{file.parent()});
  return FilePageMeta{sid, file, line_size, lines, T_Meta::latest_version};
}

/* Used to parse a segment file acccording to the v1 format.
 */
template <typename T_Meta>
class V1SegmentParser {
private:
  using Table_t = typename T_Meta::Table;

public:
  V1SegmentParser() {
    db::assert_is_table<Table_t>();
  }

  db::PresentSet<T_Meta> parse(const db::File &);

  db::segment::id get_id(const db::File &);
};

template <typename T_Meta>
db::PresentSet<T_Meta> V1SegmentParser<T_Meta>::parse(const db::File &file) {
  using db::fs::Line;
  using db::fs::Table_size;
  using Line_t = Line<Table_size<Table_t>::value(), typename T_Meta::hash_algh>;
  using db::fs::FileReader;
  using db::Buffer;
  auto present = [](const Line_t &line) {
    return line.id != db::raw::EMPTY_LINE;
  };
  //
  const size_t line_size = db::fs::Line_size<T_Meta>::value();
  constexpr size_t lines = T_Meta::extent_lines();
  //
  std::bitset<lines> res;
  FileReader fr(file);
  // TODO parse header
  // TODO advice to skip page cache
  size_t current(0);
  while (current < lines) {
    Buffer<line_size> buffer;
    fr.read(buffer);
    buffer.flip();
    Line_t line{buffer};
    res[current++] = present(line);
  }
  return db::PresentSet<T_Meta>{res};
}

/* Facade for creating and parsing segment files
 */
class Format {
private:
public:
  /* Every new segment file should be created using the latest format.
   */
  template <typename T_Meta>
  using latest = V1SegmentInit<T_Meta>;

  /* Get parser based on segment version
   */
  template <typename T_Meta>
  static constexpr V1SegmentParser<T_Meta> parser(db::segment::version) {
    // TODO version support
    return V1SegmentParser<T_Meta>{};
  }
};
}

#endif // PROJECT_FORMAT_H
