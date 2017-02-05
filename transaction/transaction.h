#ifndef DB_TX_TRANSACTION_H
#define DB_TX_TRANSACTION_H

#include "../shared/shared.h"
#include "Settings.h"

namespace tx {

class Transaction {
private:
  // journal::Journals<hash_t> &journal;
public:
  const tx::Settings settings;
  const tx::id xid;
  const journal::id jid;
  explicit Transaction(tx::id pxid, journal::id pjid, tx::Settings &&s)
      : settings(s), xid(pxid), jid(pjid) {
  }
  // Transaction(const Transaction&) = delete;
  // Transaction(const Transaction&) = delete;
  ~Transaction() {
    // journal.commit(jid);
  }
};
}
#endif // PROJECT_TRANSACTION_H
