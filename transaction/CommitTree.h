#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H

#include "../collection/CachedAllocator.h"
#include "../shared/ExtentSet.h"
#include "../shared/shared.h"
#include <stdint.h>

namespace tx {

class CommitTree {
private:
  using Allocator = sp::ParrentAllocator;
  using Extents_t = std::vector<db::HeapExtentSet>;

private:
  const db::table::id m_table;
  // db::HeapExtentSet es;
  Allocator m_allocator;

public:
  explicit CommitTree(const db::table::id &t, Allocator &&a, Extents_t &&)
      : m_table(t), m_allocator(std::move(a)) {
  }

  const db::table::id &table() const & {
    return m_table;
  }

  // void introduce(const db::ExtentSet &) {
  // }
};
}
#endif
