//
// Created by spooky on 2016-03-12.
//

#ifndef FS_SEGMENT_H
#define FS_SEGMENT_H

#include "../fs/FileWriter.h"
#include "../journal/Journals.h"
#include "../shared/Assertions.h"
#include "../shared/entities.h"
#include "PresentSet.h"
#include "Reservations.h"
#include "format/Format.h"

namespace db {
namespace fs {
/**
 * Represent a fix arena of data
 * Segment
 * |Extent 0
 * |Extent 1
 * |...
 */
template <typename Meta_t>
class Extent {
  // TODO
};

template <typename Meta_t>
class Segment {
private:
  using hash_algh = typename Meta_t::hash_algh;
  using T_Table = typename Meta_t::Table;
  using Page_t = typename Meta_t::Page;

private:
  PresentSet<Meta_t> m_lines;
  Reservations<Meta_t> m_reservations;
  SegmentFile m_file;
  Page_t m_page;
  // State
public:
  //                db::assert_is_meta<Meta_t>();
  explicit Segment(SegmentFile &&file, PresentSet<Meta_t> &&lines)
      : m_lines{std::move(lines)}, m_reservations{m_file.id(), m_lines},
        m_file{std::move(file)} {
  }

  Segment(Segment<Meta_t> &&o)
      : m_lines{std::move(o.m_lines)},
        m_reservations{std::move(o.m_reservations)},
        m_file{std::move(o.m_file)} {
  }

  Segment(const Segment &) = delete;

  const PresentSet<Meta_t> &present_set() const;
  const SegmentFile &file() const {
    return m_file;
  }
  const Reservations<Meta_t> &reservations() const {
    return m_reservations;
  }
  Reservations<Meta_t> &reservations() {
    return m_reservations;
  }
};

template <typename Meta_t>
const PresentSet<Meta_t> &Segment<Meta_t>::present_set() const {
  return m_lines;
}

namespace internal {
template <typename Meta_t>
class SegmentFileInitJournal {
private:
  using Table_t = typename Meta_t::Table;
  using hash_t = typename Meta_t::hash_algh;
  const Directory &m_root;
  journal::Journals<hash_t> &m_journal;

public:
  explicit SegmentFileInitJournal(const Directory &root,
                                  journal::Journals<hash_t> &journal)
      : m_root(root), m_journal{journal} {
    //                    db::assert_is_table<Table_t>();
  }

  SegmentFileInitJournal(const SegmentFile &) = delete;

  SegmentFileInitJournal(SegmentFile &&) = delete;

  Segment<Meta_t> create(db::segment::id sid) {
    auto jid = m_journal.template begin<Table_t>();

    m_journal.template create<Table_t>(jid, sid);

    typename db::segment::Format::latest<Meta_t> init{m_root};
    PresentSet<Meta_t> p{};

    Segment<Meta_t> result{init.create(sid), std::move(p)};
    m_journal.template commit<Table_t>(jid);
    return result;
  }
};
}
}
}

#endif // FS_SEGMENT_H
