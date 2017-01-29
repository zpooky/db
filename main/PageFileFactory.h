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
  journal::Journals<hash_t> &m_journal;

public:
  PageFileFactory(db::Context<hash_t> &ctx, db::segment::id start,
                  const db::Directory &root)
      : m_counter(start), m_root(root), m_journal(ctx.journal()) {
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
