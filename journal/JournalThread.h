#ifndef JOURNAL_THREAD_H
#define JOURNAL_THREAD_H

#include "JournalLine.h"
#include "../collection/Queue.h"
#include "../fs/FileWriter.h"

namespace journal {

template <typename hash_algh>
class JournalThread : public Consumer<JournalLine<hash_algh>> {
private:
  using SegLine = JournalLine<hash_algh>;
  const db::File m_file;
  sp::Queue<SegLine> m_queue;
  std::atomic<bool> m_interrupted;

public:
  explicit JournalThread(const db::File &seg_file)
      : m_file(seg_file), m_queue(/*empty_segment_line<hash_algh>()*/),
        m_interrupted(false) {
  }

  JournalThread(JournalThread<hash_algh> &&o)
      : m_file(o.m_file), m_queue(std::move(o.m_queue)),
        m_interrupted(o.m_interrupted.load()) {
  }

  JournalThread(const JournalThread &) = delete;

  void add(SegLine &&data) {
    m_queue.enqueue(std::move(data));
  }

  void interrupt() {
    m_interrupted.store(true);
  }

  void operator()() {
    db::FileWriter fw(m_file);
    while (!m_interrupted) {
      auto entries = m_queue.drain();
    }
  }
};
}
#endif
