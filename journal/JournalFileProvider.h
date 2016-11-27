#ifndef _JOURNAL_FILE_PROVIDER_H
#define _JOURNAL_FILE_PROVIDER_H

#include "../shared/entities.h"
#include "io/JournalFile.h"

namespace journal {
class JournalFileProvider {
private:
public:
  explicit JournalFileProvider(const db::Directory &) {
  
  }
  JournalFile& operator()(){
  }
};
}
#endif
