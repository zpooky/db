#ifndef _DB_PAGE_REPLAY_PAGE_FORMAT
#define _DB_PAGE_REPLAY_PAGE_FORMAT

#include "../../fs/FileReader.h"
#include "../../fs/Line.h"
#include "../../shared/LittleEndian.h"
#include "Format.h"
#include <iostream>

namespace page {
//TODO remove Meta_t 
template<typename Meta_t>
class ReplayPageFile {
private:
  // using Table_t = typename Meta_t::Table;
  // using hash_t = typename Meta_t::hash_algh;
  // using Line_t = db::Line<Table_t, hash_t>;
  // TODO parse endianess
  using Endianess = db::LittleEndian;

private:
  const FilePageMeta &m_meta;

public:
  explicit ReplayPageFile(const FilePageMeta &meta) : m_meta(meta) {
  }
  
  ReplayPageFile(const ReplayPageFile &&) = delete;
  ReplayPageFile(const ReplayPageFile &) = delete;

private:
  template <typename Functions>
  void apply(Functions &fs, const db::LineMeta &l) const {
    for (auto &f : fs) {
      f(l);
    }
  }

public:
  template <typename Functions>
  void replay(Functions &fs) {
    using Buffer_t = db::HeapBuffer;
    size_t lines = m_meta.lines;

    auto file = m_meta.file;
    auto parser = Format::parser<Meta_t>(file);
    // std::cout << file.path << std::endl;

    parser.parse([&](db::fs::FileReader &fr) {
      Buffer_t buffer(m_meta.line_size);
      size_t current(0);

      while (current < lines) {
        fr.read(buffer);

        if (buffer.position() == buffer.capacity()) {
          // std::cout<<current<<"\n";
          buffer.flip();
          auto meta = db::LineMeta::read<Endianess, Buffer_t>(buffer);
          // if (meta) {
          //   auto data = Table_t::template read<Endianess, Buffer_t>(buffer);
          // }
          apply(fs, meta);

          buffer.clear();
          ++current;
        }
      }
    });
  }
};
}
#endif
