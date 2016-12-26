//
// Created by spooky on 2016-07-20.
//

#ifndef PROJECT_CONTEXT_H
#define PROJECT_CONTEXT_H

#include "../journal/JournalThread.h"
#include "../journal/Journals.h"
#include "../shared/LittleEndian.h"
#include "../shared/entities.h"
#include "../shared/shared.h"
#include <thread>

namespace db {
/* Provides table-shared subsytems
 * - Segment file journal.
 *
 */
template <typename hash_t>
class Context {
private:
  const Directory m_root;

private:
  journal::JournalThread<hash_t> m_runnable;
  journal::Journals<hash_t> m_journal;
  std::thread m_thread;

public:
  explicit Context(const Directory &p_root)
      : m_root(p_root), m_runnable(p_root.cd(DirectoryName("journal"))),
        m_journal(m_runnable), m_thread([&] { m_runnable(); }) {
  }

  Context(const Context &&) = delete;

  Context(const Context &) = delete;

  ~Context() {
    m_runnable.interrupt();
    if (m_thread.joinable()) {
      m_thread.join();
    }
  }

public:
  using endianess = db::LittleEndian;

  const db::Directory root() const {
    return m_root;
  }
  journal::Journals<hash_t> &journal() {
    return m_journal;
  }
};
}
#endif // PROJECT_CONTEXT_H
