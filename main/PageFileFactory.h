#ifndef _DB_PAGE_FILE_FACTORY
#define _DB_PAGE_FILE_FACTORY

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../segment/Segment.h"
#include "../shared/IdGenerator.h"
#include "../shared/entities.h"
#include "../shared/shared.h"
#include "PageFileInitJournal.h"

namespace page {
/**
 * Transactional page file factory
 * - Statefull - keeps track of segment id
 */
template <typename Meta_t>
class PageFileFactory {
private:
  using hash_t = typename Meta_t::hash_t;

private:
  db::IdGenerator<db::segment::id> m_counter;
  const db::Directory m_root;
  const db::table::id m_table;
  journal::Journals &m_journal;

public:
  PageFileFactory(const db::TableSegment &id, journal::Journals &j,
                  const db::Directory &root)
      : m_counter(id.segment), m_root(root), m_table(id.table), m_journal(j) {
  }

  PageFileFactory(PageFileFactory<Meta_t> &&o)
      : m_counter{std::move(o.m_counter)}, m_root(o.m_root),
        m_journal(o.m_journal) {
  }

  PageFileFactory(const PageFileFactory<Meta_t> &) = delete;

  ~PageFileFactory() {
  }

  db::Segment<Meta_t> operator()() {
    PageFileInitJournal<Meta_t> init{m_root, m_journal};
    return init.create(m_counter.next());
  }
};
}
#endif
