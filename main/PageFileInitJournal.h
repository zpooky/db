#ifndef _DB_SEGMENT_FILE_INIT_JOURNAL
#define _DB_SEGMENT_FILE_INIT_JOURNAL

#include "../journal/Journals.h"
#include "../page/FilePage.h"
#include "../page/format/Format.h"
#include "../shared/PresentSet.h"
#include "../segment/Segment.h"
#include "../shared/entities.h"

namespace page {
template <typename Meta_t>
class PageFileInitJournal {
private:
  using Table_t = typename Meta_t::latest;
  using hash_t = typename Meta_t::hash_t;
  using Extent_t = db::Extent<Meta_t>;
  using Extents_t = db::Extents<Meta_t>;

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

  db::Segment<Meta_t> create(db::segment::id sid) {
    auto jid = m_journal.template begin();

    m_journal.template create<Meta_t>(jid, sid);

    typename page::Format::latest<Meta_t> init{m_root};

    m_journal.template create<Meta_t>(jid, sid);

    FilePageMeta meta = init.create(sid);

    using std::move;
    std::vector<Extent_t> xs{};
    xs.emplace_back(sid);
    Extents_t extents(move(xs));

    db::Segment<Meta_t> res{FilePage<Meta_t>(move(meta)), move(extents)};

    m_journal.template commit(jid);
    return res;
  }
};
}
#endif
