#ifndef JOURNAL_THREAD_H
#define JOURNAL_THREAD_H

#include "../collection/Queue.h"
#include "../shared/vfs.h"
#include "Consumer.h"
#include "JournalFileProvider.h"
#include "JournalLine.h"
#include "io/JournalPageBufferedFileWriter.h"

namespace journal {

/*
 * - Runs in a separate thread
 * - async reads from a queue of journal events.
 */
template <typename hash_t>
class JournalThread : public Consumer<JournalLine<hash_t>> {
private:
  using SegLine = JournalLine<hash_t>;
  JournalFileProvider<hash_t> m_provider;
  JournalPageBufferedFileWriter<hash_t> m_writer;
  sp::Queue<SegLine> m_queue;
  std::atomic<bool> m_interrupted;

public:
  explicit JournalThread(const db::Directory &root)
      : m_provider{root},
        m_writer(m_provider, vfs::sector::logical::size(root.c_str())),
        m_queue(/*empty_segment_line<hash_t>()*/), m_interrupted(false) {
    vfs::mkdir(root);
  }

  JournalThread(JournalThread<hash_t> &&o)
      : m_provider{std::move(o.m_provider)},
        m_writer{m_provider, std::move(o.m_writer)},
        m_queue(std::move(o.m_queue)), m_interrupted(o.m_interrupted.load()) {
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
      m_writer.write(m_queue.drain());
    }
    m_writer.force_flush();
  }
};
}
#endif
