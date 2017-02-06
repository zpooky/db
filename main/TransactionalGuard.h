#ifndef _DB_TX_TRANSACTIONAL_GUARD_H
#define _DB_TX_TRANSACTIONAL_GUARD_H

#include "Tx.h"
#include <algorithm>

namespace tx {

class TxGuard {
private:
  Tx *m_tx;
  db::table::id m_table;

public:
  TxGuard() : m_tx(nullptr), m_table(db::table::NO_ID) {
  }

  TxGuard(Tx &tx, CommitTree &&t) : m_tx(&tx), m_table(t.table()) {
    m_tx->introduce(std::move(t));
  }

  TxGuard(TxGuard &&o) : m_tx(nullptr), m_table(db::table::NO_ID) {
    swap(o);
  }

  TxGuard &operator=(TxGuard &&o) {
    if (this != &o) {
      assign(std::move(o));
    }
    return *this;
  }

  TxGuard(const TxGuard &) = delete;

  ~TxGuard() {
    if (m_tx) {
      m_tx->release(m_table);
      m_tx = nullptr;
    }
  }

  void swap(TxGuard &o) {
    std::swap(m_tx, o.m_tx);
    std::swap(m_table, o.m_table);
  }

private:
  void assign(TxGuard o) {
    swap(o);
  }
};

} // namespace tx

#endif
