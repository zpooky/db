#ifndef _DB_SEGMENT_FILE_INIT_JOURNAL
#define _DB_SEGMENT_FILE_INIT_JOURNAL

#include "../journal/Journals.h"
#include "../page/FilePage.h"
#include "../page/format/Format.h"
#include "../segment/Segment.h"
#include "../shared/PresentSet.h"
#include "../shared/entities.h"

namespace page {
template <typename Meta_t>
class PageFileInitJournal {
private:
  using Table_t = typename Meta_t::latest;
  constexpr static size_t lines = Meta_t::extent_lines();

private:
  const db::Directory &m_root;
  journal::Journals &m_journal;

public:
  explicit PageFileInitJournal(const db::Directory &root, journal::Journals &j)
      : m_root(root), m_journal{j} {
  }

  PageFileInitJournal(const PageFileInitJournal &) = delete;
  PageFileInitJournal(const PageFileInitJournal &&) = delete;

  ~PageFileInitJournal() {
  }

  db::Segment<Meta_t> create(db::segment::id sid) {
    auto jid = m_journal.begin(); // TODO RAII
    m_journal.template create<Meta_t>(jid, sid);
    typename page::Format::latest<Meta_t> init{m_root};
    m_journal.template create<Meta_t>(jid, sid);

    FilePageMeta meta = init.create(sid);

    using std::move;
    std::vector<db::Extent<Meta_t>> xs{};
    std::bitset<lines> b(false);
    xs.emplace_back(db::PresentSet<lines>(b, db::PageRange(0, lines)));
    db::Extents<Meta_t> extents(move(xs));

    db::Segment<Meta_t> res{meta.id, FilePage<Meta_t>(meta), move(extents)};

    m_journal.commit(jid); // TODO RAII with begin
    return res;
  }
};
}
#endif
