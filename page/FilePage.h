#ifndef PROJECT_FILE_PAGE_H
#define PROJECT_FILE_PAGE_H

#include "io/FilePageMeta.h"

namespace page {

template <typename Meta_t>
class FilePage {
private:
  FilePageMeta m_meta;

public:
  explicit FilePage(FilePageMeta &&meta) : m_meta{std::move(meta)} {
  }

public:
  db::segment::id id() const {
    return m_meta.id();
  }
};
}

#endif
