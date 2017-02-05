#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H

#include "../shared/ExtentSet.h"
#include "../shared/shared.h"
#include <stdint.h>
#include "../collection/CachedAllocator.h"

namespace tx {

class CommitTree {
private:
  const db::table::id m_table;
  db::HeapExtentSet es;

public:
public:
  explicit CommitTree(const db::table::id &table, sp::CachedAllocator &a)
      : m_table(table), es(db::segment::id(0), a) {
  }

  // void introduce(const db::ExtentSet &) {
  // }
};
}
#endif
