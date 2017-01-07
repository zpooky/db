#ifndef _DB_JOURNAL_SPECIFICATION_H
#define _DB_JOURNAL_SPECIFICATION_H

#include "../shared/shared.h"

namespace journal {

enum class Spec : uint8_t {
  SEGMENT_CREATE = 1,
  SEGMENT_UPDATE = 2,
  SEGMENT_DELETE = 3,
  PAGE_FILE_CREATE = 4,
  PAGE_EXTENT_CREATE = 5
};

template <typename Table_t>
class SegmentSpec {
private:
public:
  template <typename Buffer>
  static void create(Buffer &, const Table_t &);
};

template <typename Table_t>
class PageFileSpec {
private:
public:
  template <typename Buffer>
  static void create(Buffer &, db::segment::id);
};

template <typename Table_t>
template <typename Buffer>
void SegmentSpec<Table_t>::create(Buffer &, const Table_t &) {
  // TODO
}

template <typename Table_t>
template <typename Buffer>
void PageFileSpec<Table_t>::create(Buffer &, db::segment::id) {
  // TODO
}
}
#endif
