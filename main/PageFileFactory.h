#ifndef _DB_PAGE_FILE_FACTORY
#define _DB_PAGE_FILE_FACTORY

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../shared/entities.h"
#include "../shared/shared.h"
#include "../segment/Segment.h"
#include "PageFileInitJournal.h"
namespace page {
/**
 * Transactional page file factory
 * - Statefull - keeps track of segment id
 */
template <typename Meta_t>
class PageFileFactory {
private:
  using hash_t = typename Meta_t::hash_algh;

private:
  std::atomic<db::segment::id> m_seg_counter;
  const db::Directory m_root;
  journal::Journals<hash_t> &m_journal;

public:
  PageFileFactory(db::Context<hash_t> &ctx, db::segment::id start,
                  const db::Directory &root)
      : m_seg_counter(start), m_root(root), m_journal(ctx.journal()) {
  }

  PageFileFactory(PageFileFactory<Meta_t> &&o)
      : m_seg_counter{o.m_seg_counter.load()}, m_root(o.m_root),
        m_journal(o.m_journal) {
  }
  PageFileFactory(const PageFileFactory<Meta_t> &) = delete;

  ~PageFileFactory() {
  }

  db::fs::Segment<Meta_t> operator()() {
    PageFileInitJournal<Meta_t> init{m_root, m_journal};
    return init.create(m_seg_counter++);
  }
};
}
#endif
