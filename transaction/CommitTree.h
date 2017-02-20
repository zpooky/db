#ifndef _DB_TRANSACTION_COMMIT_TREE_H
#define _DB_TRANSACTION_COMMIT_TREE_H

#include "../collection/CachedAllocator.h"
#include "../shared/ExtentSet.h"
#include "../shared/shared.h"
#include <stdint.h>

namespace tx {

class CommitTree {
private:
  using Extents_t = std::vector<db::ExtentSet>;

private:
  const db::table::id m_table;
  Extents_t m_segments;
  sp::ParrentAllocator m_allocator;

public:
  explicit CommitTree(const db::table::id &t, sp::ParrentAllocator &&a, Extents_t &&s)
      : m_table(t), m_segments(std::move(s)), m_allocator(std::move(a)) {
  }

  const db::table::id &table() const & {
    return m_table;
  }

  void introduce(const db::ExtentSet &e) {
  }
};
} // namespace tx
#endif
