#ifndef _DB_TX_TRANSACTIONAL_GUARD_H
#define _DB_TX_TRANSACTIONAL_GUARD_H

#include "Tx.h"
#include <algorithm>

namespace tx {

template <typename hash_t>
class TransactionalGuard {
private:
  Tx<hash_t> *m_tx;
  const db::table::id m_table;

public:
  TransactionalGuard(Tx<hash_t> &tx, CommitTree &&t)
      : m_tx(&tx), m_table(t.table()) {
    m_tx->introduce(std::move(t));
  }

  TransactionalGuard(TransactionalGuard &&o)
      : m_tx(nullptr), m_table(o.m_table) {
    std::swap(m_tx, o.m_tx);
  }

  TransactionalGuard(const TransactionalGuard &) = delete;

  ~TransactionalGuard() {
    if (m_tx) {
      m_tx->release(m_table);
      m_tx = nullptr;
    }
  }
};

} // namespace tx

#endif
