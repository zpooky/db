//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_SEGMENTFILE_H
#define PROJECT_SEGMENTFILE_H

#include "../../shared/entities.h"
#include "../../shared/shared.h"
#include <cstddef>
#include <utility>
#include <set>

namespace page {
class FilePageMeta {
private:
  const db::segment::id m_id;
  const db::File m_file;
  const size_t m_line_size;
  const size_t m_number;
  const db::table::version m_version;
private:
  // #<{(|*
  //  * collection of uncommited journals
  //  |)}>#
  // std::set<journal::id> m_uncommited;

public:
  FilePageMeta(db::segment::id id, const db::File &file, size_t line_size,
              size_t number, db::table::version version)
      : m_id{id}, m_file{file}, m_line_size{line_size}, m_number{number},
        m_version{version} {
  }

  FilePageMeta(const FilePageMeta &) = delete;

  FilePageMeta(FilePageMeta &&o)
      : m_id{o.m_id}, m_file{std::move(o.m_file)}, m_line_size{o.m_line_size},
        m_number{o.m_number}, m_version{o.m_version} {
  }

  const db::File &file() const {
    return m_file;
  }
  auto id() const {
    return m_id;
  }
};
}
#endif // PROJECT_SEGMENTFILE_H
