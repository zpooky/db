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

private:
  const db::File m_file;

public:
  explicit V1SegmentParser(const db::File &file) : m_file(file) {
    db::assert_is_table<Table_t>();
  }

  // db::PresentSet<Meta_t> parse(const db::File &);

  template <typename Function>
  void parse(Function);
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

  // #<{(| Get parser based on segment version
  //  |)}>#
  // template <typename Meta_t>
  // static constexpr V1SegmentParser<Meta_t> parser(db::segment::version) {
  //   // TODO version support
  //   return V1SegmentParser<Meta_t>{};
  // }

  template <typename Meta_t>
  static V1SegmentParser<Meta_t> parser(const db::File &file) {
    // TODO version support
    return V1SegmentParser<Meta_t>{file};
  }
};

template <typename Meta_t>
FilePageMeta V1SegmentInitializer<Meta_t>::create(db::segment::id sid) {
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  using Line_t = typename db::Line<Table_t, hash_t>;
  db::Filename filename{db::Segment_name::name(sid)};

  using capacity = unsigned long long;
  constexpr size_t line_size = Line_t::size();
  constexpr size_t lines = Meta_t::extent_lines();
  std::cout << "line_size:" << line_size << "lines" << lines << "\n";
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
template <typename Function>
void V1SegmentParser<Meta_t>::parse(Function f) {
  db::fs::FileReader fr(m_file);
  // TODO headers stuff
  f(fr);
}
}

#endif // PROJECT_FORMAT_H
