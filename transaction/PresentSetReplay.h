#ifndef DB_TX_PRESENT_SET_H
#define DB_TX_PRESENT_SET_H

#include "../fs/Line.h"

namespace tx {
class PresentSetReplay {
private:
  db::table::id m_table;

public:
  explicit PresentSetReplay(const db::table::id &t)
      : m_table(t) {
  }
  void operator()(const db::LineMeta &) {
  }
};
}
#endif
