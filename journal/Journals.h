//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../collection/Queue.h"
#include "../shared/IdGenerator.h"
#include "Consumer.h"
#include "JournalLine.h"
#include "Specification.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <utility>

namespace journal {
using Consumer_t = Consumer<JournalLine>;

class Journals {
private:
private:
  Consumer_t &m_consumer;
  db::IdGenerator<journal::id> m_id;

public:
  Journals(Consumer_t &c, journal::id start = journal::START_ID)
      : m_consumer(c), m_id{start} {
    assert(start != journal::NO_ID);
  }

  Journals(Journals &&o) : m_consumer(o.m_consumer), m_id(std::move(o.m_id)) {
  }

  Journals(const Journals &) = delete;

  ~Journals() {
  }

public:
  /**
   * Journal type: BEGIN
   * Creates a new journal transaction.
   */
  journal::id begin() {
    auto journal_id(m_id.next());
    m_consumer.add(journal::line::begin(journal_id));
    return journal_id;
  }
  /**
   * Journal Type: ENTRY
   * Journal entry for CREATE line
   */
  template <typename Meta_t>
  void create(journal::id jid,
              const db::Reservation<typename Meta_t::latest> &r,
              const typename Meta_t::latest &t) {
    using Table_t = typename Meta_t::latest;
    db::HeapBuffer b(1); // TODO
    SegmentSpec<Table_t>::create(b, t);
    constexpr auto table = Meta_t::table_name();
    constexpr auto type = EntryType::LINE;
    m_consumer.add(journal::line::create(jid, table, type, std::move(b)));
  }
  /**
   * Journal Type: ENTRY
   * Journal entry for CREATE segment file
   */
  template <typename Meta_t>
  void create(journal::id jid, db::segment::id sid) {
    using Table_t = typename Meta_t::latest;
    db::HeapBuffer b(1); // TODO
    PageFileSpec<Table_t>::create(b, sid);
    constexpr auto table = Meta_t::table_name();
    constexpr auto type = EntryType::SEGMENT;
    m_consumer.add(journal::line::create(jid, table, type, std::move(b)));
  }

  void rollback(const journal::id &) {
  }
  /**
   * Journal TYPE: COMMIT
   * Journal entry for commit journal transaction
   */
  void commit(const journal::id &id) {
    m_consumer.add(journal::line::commit(id));
  }
};
}

#endif // PROJECT_SEGMENTJOURNAH
