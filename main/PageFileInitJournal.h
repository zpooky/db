#ifndef _DB_SEGMENT_FILE_INIT_JOURNAL
#define _DB_SEGMENT_FILE_INIT_JOURNAL

#include "../journal/Journals.h"
#include "../page/FilePage.h"
#include "../page/format/Format.h"
#include "../segment/PresentSet.h"
#include "../segment/Segment.h"
#include "../shared/entities.h"

namespace page {
template <typename Meta_t>
class PageFileInitJournal {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;

private:
  const db::Directory &m_root;
  journal::Journals<hash_t> &m_journal;

public:
  explicit PageFileInitJournal(const db::Directory &root,
                               journal::Journals<hash_t> &journal)
      : m_root(root), m_journal{journal} {
  }

  PageFileInitJournal(const PageFileInitJournal &) = delete;
  PageFileInitJournal(const PageFileInitJournal &&) = delete;

  db::fs::Segment<Meta_t> create(db::segment::id sid) {
    auto jid = m_journal.template begin<Meta_t>();

    m_journal.template create<Meta_t>(jid, sid);

    typename page::Format::latest<Meta_t> init{m_root};

    m_journal.template create<Meta_t>(jid, sid);

    db::PresentSet<Meta_t> p{};
    FilePageMeta meta = init.create(sid);
    db::fs::Segment<Meta_t> res{FilePage<Meta_t>(std::move(meta)),
                                std::move(p)};

    m_journal.template commit<Meta_t>(jid);
    return res;
  }
};
}
#endif
