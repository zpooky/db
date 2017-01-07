#ifndef DB_TX_TRANSACTION_H
#define DB_TX_TRANSACTION_H

#include "Settings.h"
#include "../shared/shared.h"

namespace tx {

class Transaction {
private:
  // journal::Journals<hash_t> &journal;
public:
  const tx::Settings settings;
  const db::transaction::id tid;
  const journal::id jid;
  explicit Transaction(journal::id id, tx::Settings &&s)
      : settings(s), tid(0), jid(id) {
  }
  // Transaction(const Transaction&) = delete;
  ~Transaction() {
    // journal.commit(jid);
  }
};
}
#endif // PROJECT_TRANSACTION_H
