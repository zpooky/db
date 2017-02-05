#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H

#include "../collection/CachedAllocator.h"
#include "../shared/ExtentSet.h"
#include "../shared/shared.h"
#include <stdint.h>

namespace tx {

class CommitTree {
private:
  const db::table::id m_table;
  // db::HeapExtentSet es;
  sp::CachedAllocator m_allocator;

public:
public:
  explicit CommitTree(const db::table::id &table)
      : m_table(table), m_allocator(0) {
  }

  const db::table::id &table() const & {
    return m_table;
  }

  // void introduce(const db::ExtentSet &) {
  // }
};
}
#endif
