//
// Created by spooky on 2016-07-20.
//

#ifndef PROJECT_CONTEXT_H
#define PROJECT_CONTEXT_H

#include "../journal/JournalThread.h"
#include "../journal/Journals.h"
#include "../shared/Configuration.h"
#include "../shared/IdGenerator.h"
#include "../shared/LittleEndian.h"
#include "../shared/entities.h"
#include "../shared/shared.h"
#include <thread>

namespace db {
/* Provides table-shared subsytems
 * - Segment file journal.
 *
 */
class Context {
private:
  Configuration m_config;

private:
  journal::JournalThread m_runnable;
  journal::Journals m_journal;
  std::thread m_thread;
  db::IdGenerator<db::table::id> m_table_ids;

public:
  explicit Context(const Configuration &conf)
      : m_config(conf), m_runnable(conf.root.cd("journal")),
        m_journal(m_runnable), m_thread([&] { m_runnable(); }),
        m_table_ids{db::table::START_ID} {
  }

  Context(const Context &&) = delete;

  Context(const Context &) = delete;

  Context &operator=(const Context &) = delete;
  Context &operator=(const Context &&) = delete;

  ~Context() {
    m_runnable.interrupt();
    if (m_thread.joinable()) {
      m_thread.join();
    }
  }

public:
  using endianess = db::LittleEndian;

  const Configuration &config() const {
    return m_config;
  }
  journal::Journals &journal() {
    return m_journal;
  }

  db::table::id next_id() {
    return m_table_ids.next();
  }
};
}
#endif // PROJECT_CONTEXT_H
