#ifndef JOURNAL_THREAD_H
#define JOURNAL_THREAD_H

#include "../collection/Queue.h"
#include "Consumer.h"
#include "JournalFileProvider.h"
#include "JournalLine.h"
#include "io/JournalPageBufferedFileWriter.h"

namespace journal {

/*
 * - Runs in a separate thread
 * - async reads from a queue of journal events.
 * - Inteface to query if a transa
 */
template <typename hash_algh>
class JournalThread : public Consumer<JournalLine<hash_algh>> {
private:
  using SegLine = JournalLine<hash_algh>;
  JournalFileProvider m_provider;
  JournalPageBufferedFileWriter<hash_algh> m_writer;
  sp::Queue<SegLine> m_queue;
  std::atomic<bool> m_interrupted;

public:
  explicit JournalThread(const db::Directory &journal_root)
      : m_provider{journal_root}, m_writer(m_provider),
        m_queue(/*empty_segment_line<hash_algh>()*/), m_interrupted(false) {
  }

  JournalThread(JournalThread<hash_algh> &&o)
      : m_writer(o.m_writer), m_queue(std::move(o.m_queue)),
        m_interrupted(o.m_interrupted.load()) {
  }

  JournalThread(const JournalThread &) = delete;

  ~JournalThread() {
    // TODO wait on thread stop
  }

  void add(SegLine &&data) {
    m_queue.enqueue(std::move(data));
  }

  void interrupt() {
    m_interrupted.store(true);
  }

  void operator()() {
    while (!m_interrupted) {
      auto entries = m_queue.drain();
      m_writer.write(entries);
    }
  }
};
}
#endif
