//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_FORMAT_H
#define PROJECT_FORMAT_H

#include "../../fs/FileReader.h"
#include "../../fs/FileWriter.h"
#include "../../fs/Line.h"
#include "../../segment/PresentSet.h"
#include "../../shared/Assertions.h"
#include "../../shared/LittleEndian.h"
#include "../../shared/conversions.h"
#include "../../shared/entities.h"
#include "../../shared/vfs.h"
#include "../io/FilePageMeta.h"
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
template <typename Meta_t>
class V1SegmentInitializer {
private:
  using Table_t = typename Meta_t::Table;
  const db::Directory m_root;

public:
  explicit V1SegmentInitializer(const db::Directory &root) : m_root(root) {
    db::assert_is_table<Table_t>();
  }
  /**
   * Creates an empty segmentfile
   */
  FilePageMeta create(db::segment::id);
};

/* Used to parse a segment file acccording to the v1 format.
 */
template <typename Meta_t>
class V1SegmentParser {
private:
  using Table_t = typename Meta_t::Table;

public:
  V1SegmentParser() {
    db::assert_is_table<Table_t>();
  }

  db::PresentSet<Meta_t> parse(const db::File &);
};

/* Facade for creating and parsing segment files
 */
class Format {
private:
public:
  /* Every new segment file should be created using the latest format.
   */
  template <typename Meta_t>
  using latest = V1SegmentInitializer<Meta_t>;

  /* Get parser based on segment version
   */
  template <typename Meta_t>
  static constexpr V1SegmentParser<Meta_t> parser(db::segment::version) {
    // TODO version support
    return V1SegmentParser<Meta_t>{};
  }
};

template <typename Meta_t>
FilePageMeta V1SegmentInitializer<Meta_t>::create(db::segment::id sid) {
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  db::Filename filename{db::Segment_name::name(sid)};

  using capacity = unsigned long long;
  constexpr size_t line_size = db::Line<Table_t, hash_t>::size();
  constexpr size_t lines = Meta_t::extent_lines();
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
  return FilePageMeta{sid, file, line_size, lines, Table_t::latest_version};
}

template <typename Meta_t>
db::PresentSet<Meta_t> V1SegmentParser<Meta_t>::parse(const db::File &file) {
  using hash_t = typename Meta_t::hash_algh;
  using Line_t = db::Line<Table_t, hash_t>;
  using Endianess = db::LittleEndian;
  //
  auto is_present = [](const Line_t &line) {
    return line.id != db::raw::EMPTY_LINE;
  };
  // TODO read header and determine version then line size
  constexpr size_t line_size = Line_t::size();
  using Buffer_t = db::Buffer<line_size>;
  // TODO dynamic lines
  constexpr size_t lines = Meta_t::extent_lines();
  //
  std::bitset<lines> res;
  db::fs::FileReader fr(file);
  // TODO parse header
  // TODO advice to skip page cache
  size_t current(0);
  Buffer_t buffer;
  while (current < lines) {
    fr.read(buffer);
    buffer.flip();
    // TODO parse endianess
    Line_t line = Line_t::template read<Endianess, Buffer_t>(buffer);
    res[current++] = is_present(line);
    buffer.clear();
  }
  return db::PresentSet<Meta_t>{res};
}
}

#endif // PROJECT_FORMAT_H
