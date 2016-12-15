//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_SEGMENTFILE_H
#define PROJECT_SEGMENTFILE_H

#include "../../shared/entities.h"
#include "../../config/Configuration.h"
#include <cstddef>
#include <utility>

namespace db {
namespace fs {
class SegmentFile {
private:
  const db::segment::id m_id;
  File m_file;
  const size_t m_line_size;
  const size_t m_number;

public:
  SegmentFile(db::segment::id id, const File &file, size_t line_size,
              size_t number)
      : m_id{id}, m_file{file}, m_line_size{line_size}, m_number{number} {
  }

  SegmentFile(const SegmentFile &) = delete;

  SegmentFile(SegmentFile &&o)
      : m_id{o.m_id}, m_file{std::move(o.m_file)}, m_line_size{o.m_line_size},
        m_number{o.m_number} {
  }

  const File &file() const {
    return m_file;
  }
  auto id() const {
    return m_id;
  }
};
}
}
#endif // PROJECT_SEGMENTFILE_H
