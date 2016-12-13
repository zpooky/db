//
// Created by spooky on 2016-03-03.
//

#ifndef DB_JOURNAL_H
#define DB_JOURNAL_H

#include "../shared/Assertions.h"
#include "../shared/fs.h"

namespace journal {
enum class Type : uint8_t {
  START = 0,
  UPDATE = 1,
  COMMIT = 2,
  ABORT = 3,
  CHECKPOINT = 4
};

template <journal::Type TYPE>
struct Entry {
  Type get_type() const {
    return TYPE;
  }
};

class Journal {
private:
  Journal(const Journal &) = delete;

public:
  Journal() {
  }

  journal_id begin();
  
  template<typename t_Table>
  void prepare(journal_id, const db::Reservation<t_Table> &);

  void commit(journal_id);
};

Journal &instance();

Journal &instance() {
  static Journal inst{};
  return inst;
}

journal_id Journal::begin() {
  return 1l;
}

template<typename t_Table>
void Journal::prepare(journal_id id, const db::Reservation<t_Table> &) {
}

void Journal::commit(journal_id ) {
}
}

#endif // DB_JOURNAL_H
