//
// Created by spooky on 2016-04-04.
//

#ifndef PROJECT_SEGMENTJOURNAL_H
#define PROJECT_SEGMENTJOURNAL_H

#include "../collection/Queue.h"
#include "../fs/FileWriter.h"
#include "../shared/fs.h"
#include "../shared/hash.h"
#include "JournalLine.h"
#include "Consumer.h"
#include <atomic>
#include <thread>
#include <utility>

namespace journal {
using std::atomic;

template <typename hash_algh>
using Consumer_t = Consumer<JournalLine<hash_algh>>;

template <typename hash_algh>
class Journals {
private:
  Consumer_t<hash_algh> &m_consumer;
  atomic<journal_id> m_counter;

public:
  explicit Journals(Consumer_t<hash_algh> &consumer, journal_id start = 0)
      : m_consumer(consumer), m_counter{start} {
    //                db::assert_is_meta<T_Meta>();
  }

  Journals(Journals &&o)
      : m_consumer(o.m_consumer), m_counter(o.m_counter.load()) {
  }

  Journals(const Journals &) = delete;

private:
  JournalLine<hash_algh> line(journal_id id, State state) const {
    name_type name{0};
    segment_id idx = 0;
    return segment_line<hash_algh>(id, name, idx, state);
  }


public:
  journal_id begin(const name_type &name, segment_id idx) {
    auto id = ++m_counter;
    m_consumer.add(line(id, name, idx));
    return id;
  }

  //            void prepare(journal_id id) {
  //                m_consumer.add(line(id, State::PREPARED));
  //            }
  // 
  void rollback() {
  }

  void commit(journal_id id) {
    m_consumer.add(line(id, State::COMMIT));
  }
};
}

#endif // PROJECT_SEGMENTJOURNAH
