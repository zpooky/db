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

  Journals(const Journals &) = delete;

public:
  template <typename Table_t>
  journal::id begin() {
    auto journal_id = ++m_counter;
    m_consumer.add(xbegin<hash_t>(journal_id, Table_t::table_name()));
    return journal_id;
  }
  template <typename Table_t>
  void create(const db::Reservation<Table_t> &r, const Table_t &t) {
    // m_consumer.add(create());
  }

  template <typename Table_t>
  void create(journal::id jid, db::segment::id sid) {
    // m_consumer.add(create());
  }

  // void rollback() {
  // }

  template <typename Table_t>
  void commit(journal::id id) {
    m_consumer.add(commitx<hash_t>(id, Table_t::table_name()));
  }
};
}

#endif // PROJECT_SEGMENTJOURNAH
