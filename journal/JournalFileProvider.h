#ifndef _JOURNAL_FILE_PROVIDER_H
#define _JOURNAL_FILE_PROVIDER_H

#include "../shared/entities.h"
#include "io/JournalFile.h"
#include <vector>

namespace journal {
class JournalFileProvider {
private:
  std::vector<db::File> m_journal;

public:
  explicit JournalFileProvider(const db::Directory &root)
      : m_journal{db::files(root)} {
  }
  JournalFile &operator()() {
  }
};
}
#endif
