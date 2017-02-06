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
class JournalThread : public Consumer<JournalLine> {
private:
  using SegLine = JournalLine;

private:
  JournalFileProvider m_provider;
  JournalPageBufferedFileWriter m_writer;
  // sp::con::Queue<SegLine> m_queue;
  std::atomic<bool> m_interrupted;

public:
  explicit JournalThread(const db::Directory &root)
      : m_provider{root},
        m_writer(m_provider, vfs::sector::logical::size(root.c_str())),
        m_interrupted(false) {
    vfs::mkdir(root);
  }

  JournalThread(JournalThread &&o)
      : m_provider{std::move(o.m_provider)},
        m_writer{m_provider, std::move(o.m_writer)},
        m_interrupted(o.m_interrupted.load()) {
  }

  JournalThread(const JournalThread &) = delete;

  ~JournalThread() {
    // TODO wait on thread stop
  }

  void add(SegLine &&data) {
    // m_queue.enqueue(std::move(data));
  }

  void interrupt() {
    m_interrupted.store(true);
  }

  void operator()() {
    while (!m_interrupted) {
      // m_writer.write(m_queue.drain());
    }
    m_writer.force_flush();
  }
};
}
#endif
