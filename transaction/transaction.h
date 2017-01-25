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
  const tx::id tid;
  const journal::id jid;
  explicit Transaction(tx::id p_tid, journal::id p_jid, tx::Settings &&s)
      : settings(s), tid(p_tid), jid(p_jid) {
  }
  // Transaction(const Transaction&) = delete;
  // Transaction(const Transaction&) = delete;
  ~Transaction() {
    // journal.commit(jid);
  }
};
}
#endif // PROJECT_TRANSACTION_H
