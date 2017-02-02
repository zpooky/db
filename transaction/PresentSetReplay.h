#ifndef DB_TX_PRESENT_SET_H
#define DB_TX_PRESENT_SET_H

#include "../fs/Line.h"

namespace tx {
class PresentSetReplay {
private:
  db::segment::id m_segment;

public:
  explicit PresentSetReplay(const db::segment::id &segment)
      : m_segment(segment) {
  }
  void operator()(const db::LineMeta &line) {
  }
};
}
#endif
