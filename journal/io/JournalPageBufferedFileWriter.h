//
// Created by spooky on 2016-10-06.
//

#ifndef PROJECT_PAGEBUFFEREDFILEWRITER_H
#define PROJECT_PAGEBUFFEREDFILEWRITER_H

#include "../../fs/FileWriter.h"
#include "../JournalFileProvider.h"
#include "../JournalLine.h"
#include "JournalFile.h"
#include <algorithm>
#include <vector>

/**
 * Writes in a page sized and aligned buffer.
 * To avoid the reread of page frames, the buffer is written all ways at once
 *  meaning there is no point of reading the page before replacing it fully.
 * The same data is written to the same page if a force full flush is triggered.
 * Only when a page buffer is full create a new buffer and write to the next
 *  page frame.
 * Uninitialized data is represented as zeroes.
 *
 * A write will trigger a buffer write and flush:
 * - if the page buffer is full
 * - if a flush msg is received
 * If a write occurs where none of thease two conditions are met no fs write is
 *  performed the data is only buffered
 * either until the next write and the conditions are met or when the
 *  force_flash function is called.
 * Individual lines may be reordered before a commit(flush) event.
 */
namespace journal {

template <typename hash_algh>
class JournalPageBufferedFileWriter;

template <typename hash_algh>
using JPBFW = JournalPageBufferedFileWriter<hash_algh>;

template <typename hash_algh>
class JournalPageBufferedFileWriter {
private:
  JournalFileProvider &m_provider;
  JournalFile &m_file;

  db::AlignedBuffer m_buffer;
  db::DirectFileWriter m_writer;

  // TODO keep track of Pending entres(not yet commited)
  // if this is empty we can insert a safepoint
  // (this is not enough since we need to ensure
  // that the page cache in the db is also flushed)
  // std::vector<journal_id> id m_pending;
  using JLine = JournalLine<hash_algh>;

public:
  explicit JournalPageBufferedFileWriter(JournalFileProvider &provider)
      : m_provider{provider}, m_file{m_provider()},
        m_buffer{m_file.sector_size}, m_writer(m_file.file) {
  }

  JournalPageBufferedFileWriter(const JPBFW<hash_algh> &&) = delete;

  JournalPageBufferedFileWriter(const JPBFW<hash_algh> &) = delete;

  JPBFW<hash_algh> &operator=(const JPBFW<hash_algh> &&) = delete;

  JPBFW<hash_algh> &operator=(const JPBFW<hash_algh> &) = delete;
  ~JournalPageBufferedFileWriter() {
    force_flush();
  }

  /* TODO int is placeholder
   * return the id of the forced flush events?
   */
  void write(std::vector<JLine> &events) {
    assert(!events.empty());
    auto flush_ids = sort(events);
    bool require_flush(false);

    for (auto &event : events) {
      // m_buffer.write(event);
      if (event.state == journal::Type::COMMIT) {
        require_flush = true;
      }

      write(event);

      if (m_buffer.is_full()) {
        force_flush();
        m_buffer.clear();
        // We have flushed thease commits
        // so no need to flush at the end
        require_flush = false;
      }
    }

    if (require_flush) {
      force_flush();
    }
  }

private:
  void write(const JLine &line) {
  }

  void force_flush() {
    if (!m_buffer.is_empty()) {
      m_buffer.flip();
      m_writer.write(m_buffer);
      m_writer.flush();
      m_buffer.flip();
    }
  }

  auto sort(std::vector<JLine> &events) const {
    // TODO implement real
    auto cmp = [](JLine f, JLine s) { return false; };
    std::sort(begin(events), end(events), cmp);
    return std::vector<journal::id>{};
  }
};
}

#endif // PROJECT_PAGEBUFFEREDFILEWRITER_H
