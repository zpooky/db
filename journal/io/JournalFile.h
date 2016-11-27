//
// Created by spooky on 2016-10-06.
//

#ifndef PROJECT_JOURNALFILE_H
#define PROJECT_JOURNALFILE_H

#include "../../fs/FileWriter.h"

namespace journal {
struct JournalFile {
  const size_t start;
  const size_t capacity;
  const size_t sector_size;

  const db::File file;
  explicit JournalFile(size_t p_start, size_t p_capacity, size_t p_sector_size,
                       const db::File &f)
      : start{p_start}, capacity{p_capacity}, sector_size{p_sector_size},
        file{f} {
  }
};
}

#endif // PROJECT_JOURNALFILE_H
