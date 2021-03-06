#ifndef _DB_PAGE_MAX_ID_REPLAY
#define _DB_PAGE_MAX_ID_REPLAY

#include "../fs/Line.h"
#include "../page/io/FilePageMeta.h"
#include "../shared/shared.h"

namespace page {
/**
 * determine what will be the next raw::id
 * by replaying all lines.
 */
class MaxIdReplay {
private:
  db::raw::id m_greatest;

public:
  MaxIdReplay() : m_greatest(db::raw::EMPTY) {
  }

  MaxIdReplay(const MaxIdReplay &) = delete;
  MaxIdReplay(const MaxIdReplay &&) = delete;
  ~MaxIdReplay() {
  }

  void operator()(const FilePageMeta &, const db::LineMeta &line) {
    m_greatest = std::max(line.id, m_greatest);
  }

  auto next() const {
    return db::raw::id(m_greatest + 1);
  }
};
}

#endif
