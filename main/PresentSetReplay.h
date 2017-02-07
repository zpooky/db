#ifndef DB_TX_PRESENT_SET_H
#define DB_TX_PRESENT_SET_H

#include "../fs/Line.h"
#include "../page/io/FilePageMeta.h"
#include "../shared/ExtentSet.h"
#include "../shared/PresentSet.h"
#include "../transaction/CommitTree.h"
#include "ExtentsBuilder.h"

namespace tx {

template <typename Meta_t>
class PresentSetReplay {
private:
  static constexpr size_t lines = Meta_t::extent_lines();
  using Allocator = sp::CachedAllocator;

private:
  db::ExtentsBuilder<lines> m_builders;
  std::vector<db::HeapExtentSet> m_extents;
  Allocator m_allocator;
  db::table::id m_table;

public:
  explicit PresentSetReplay(const db::table::id &t)
      : m_builders(), m_extents(), m_allocator(0), m_table(t) {
  }

  PresentSetReplay(PresentSetReplay &&o)
      : m_builders(std::move(m_builders)), m_extents(std::move(m_extents)),
        m_allocator(std::move(o.m_allocator)), m_table(o.m_table) {
  }

  PresentSetReplay(const PresentSetReplay &) = delete;

  void operator()(const page::FilePageMeta &m, const db::LineMeta &l) {
    m_builders.next(bool(l));
  }

  CommitTree build() {
    using std::move;
    return CommitTree(m_table, move(m_allocator), move(m_extents));
  }
};
}
#endif
