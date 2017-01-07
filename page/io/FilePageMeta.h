//
// Created by spooky on 2016-07-05.
//

#ifndef PROJECT_SEGMENTFILE_H
#define PROJECT_SEGMENTFILE_H

#include "../../shared/entities.h"
#include "../../shared/shared.h"

namespace page {
struct FilePageMeta {
  const db::segment::id id;
  const db::File file;
  const db::raw::size line_size;
  /**
   * number of possible lines.
   * file size based on header + line_size*number
   *
   * TODO allow resize of existing FilePage using extents
   *      this field should not be const
   */
  const size_t lines;
  const db::table::version version;
private:
  // #<{(|*
  //  * collection of uncommited journals
  //  |)}>#
  // std::set<journal::id> m_uncommited;

public:
  FilePageMeta(db::segment::id p_id, const db::File &p_file, db::raw::size p_line_size,
              size_t p_lines, db::table::version p_version)
      : id{p_id}, file{p_file}, line_size{p_line_size}, lines{p_lines},
        version{p_version} {
  }
};
}
#endif // PROJECT_SEGMENTFILE_H
