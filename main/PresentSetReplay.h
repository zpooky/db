#ifndef DB_TX_PRESENT_SET_H
#define DB_TX_PRESENT_SET_H

#include "../fs/Line.h"
#include "../page/io/FilePageMeta.h"
#include "../shared/Configuration.h"
#include "../shared/ExtentSet.h"
#include "../shared/PresentSet.h"
#include "../transaction/CommitTree.h"
#include "ExtentsBuilder.h"
#include "Util.h"

namespace tx {

/*
 * instance is shared between all segment files
 */
template <typename Meta_t>
class PresentSetReplay {
private:
  static constexpr size_t lines = db::Configuration::extent_lines();

private:
  db::ExtentsBuilder<lines> m_builders;
  std::vector<db::HeapExtentSet> m_extents;
  sp::ParrentAllocator m_allocator;
  db::Configuration m_config;
  db::table::id m_table;

public:
  explicit PresentSetReplay(const db::Configuration &c, const db::table::id &t)
      : m_builders(), m_extents(), m_allocator(), m_config(c), m_table(t) {
  }

  PresentSetReplay(PresentSetReplay &&o)
      : m_builders(std::move(m_builders)), m_extents(std::move(m_extents)),
        m_allocator(std::move(o.m_allocator)), m_table(o.m_table) {
  }

  PresentSetReplay(const PresentSetReplay &) = delete;

  void operator()(const page::FilePageMeta &m, const db::LineMeta &l) {
    const auto segment(m.id);
    if (segment != m_builders.segment()) {
      db::ExtentsBuilder<lines> n(segment);
      m_builders.swap(n);
      size_t actual_extents(0); // TODO actual impl
      size_t extents(std::max(m_config.extents, actual_extents));
      auto child = m_allocator.child_st<db::HeapExtentSet::AllocType>(extents);
      using std::move;
      m_extents.emplace_back(segment, m_config.extents, move(child),
                             db::segment::present_sets(n.builders()));
    }
    m_builders.next(bool(l));
  }

  CommitTree build() {
    using std::move;
    return CommitTree(m_table, move(m_allocator), move(m_extents));
  }
};
}
#endif
