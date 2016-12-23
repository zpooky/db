#ifndef _JOURNAL_FILE_PROVIDER_H
#define _JOURNAL_FILE_PROVIDER_H

#include "../shared/entities.h"
#include "Journals.h"
#include "io/JournalFile.h"
#include <vector>

namespace journal {
/**
 * A pool of journal files
 */
template <typename hash_t>
class JournalFileProvider {
private:
  /**
   * Journal files which are ready to be used.
   */
  std::vector<db::File> m_free_journals;
  /**
   * Journal files which still have fully uncommited data in them.
   */
  std::vector<JournalFile> m_dirty;

public:
  explicit JournalFileProvider(const db::Directory &root)
      : m_free_journals(db::files(root)), m_dirty{} {
    // TODO make better
    for (const auto &file : m_free_journals) {
      zero(file);
    }
  }

  JournalFileProvider(JournalFileProvider &&o)
      : m_free_journals{std::move(o.m_free_journals)},
        m_dirty{std::move(o.m_dirty)} {
  }
  JournalFileProvider(const JournalFileProvider &) = delete;

private:
  /**
   * TODO create WAL journal for zeroing
   * the journal file. if crash before commit resume in the replay stage.
   */
  void zero(const db::File &) {
  }

public:
  /*
   * f :: FileWriter& -> Unit
   */
  template <typename Function>
  void with(Function f) {
    // db::FileWriter fw{current()};
    // auto commited = f();
    // current().add(commited);
    // if(fw.is_full()){
    //   new_current();
    // }
    // // std::vector finalized = scan(commited);
    // // finalized.for_each([](const JournalFile& f){
    // //   zero(f.file());
    // // });
    // // add_free(finalized);
  }
};
}
#endif
