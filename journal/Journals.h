//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../collection/Queue.h"
#include "../fs/FileWriter.h"
#include "../shared/hash.h"
#include "Consumer.h"
#include "JournalLine.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <utility>

namespace journal {
template <typename hash_t>
using Consumer_t = Consumer<JournalLine<hash_t>>;

template <typename hash_t>
class Journals {
private:
  Consumer_t<hash_t> &m_consumer;
  std::atomic<journal::id> m_counter;

public:
  explicit Journals(Consumer_t<hash_t> &consumer,
                    journal::id start = journal::START_ID)
      : m_consumer(consumer), m_counter{start} {
    assert(start != journal::NO_ID);
  }

  Journals(Journals<hash_t> &&o)
      : m_consumer(o.m_consumer), m_counter(o.m_counter.load()) {
  }

  Journals(const Journals<hash_t> &) = delete;

public:
  /**
   * Journal type: BEGIN
   * Creates a new journal transaction.
   */
  journal::id begin() {
    auto journal_id = ++m_counter;
    m_consumer.add(journal::line::begin<hash_t>(journal_id));
    return journal_id;
  }
  /**
   * Journal Type: ENTRY
   * Journal entry for CREATE line
   */
  template <typename Meta_t>
  void create(journal::id jid, const db::Reservation<typename Meta_t::Table> &r,
              const typename Meta_t::Table &t) {
    db::HeapBuffer b;
    // TODO populate buffer
    constexpr auto table = Meta_t::table_name();
    auto type = EntryType::LINE;
    m_consumer.add(
        journal::line::create<hash_t>(jid, table, type, std::move(b)));
  }
  /**
   * Journal Type: ENTRY
   * Journal entry for CREATE segment file
   */
  template <typename Meta_t>
  void create(journal::id jid, db::segment::id sid) {
    db::HeapBuffer b;
    // TODO populate buffer
    constexpr auto table = Meta_t::table_name();
    auto type = EntryType::SEGMENT;
    m_consumer.add(
        journal::line::create<hash_t>(jid, table, type, std::move(b)));
  }

  // void rollback() {
  // }
  /**
   * Journal TYPE: COMMIT
   * Journal entry for commit journal transaction
   */
  void commit(journal::id id) {
    m_consumer.add(journal::line::commit<hash_t>(id));
  }
};
}

#endif // PROJECT_SEGMENTJOURNAH
