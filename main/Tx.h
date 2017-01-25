#ifndef _TX_TRANSACTION_TX
#define _TX_TRANSACTION_TX

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../shared/shared.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"

namespace tx {

template <typename hash_t>
class Tx {
private:
  journal::Journals<hash_t> &m_journal;
  Transactionx<10> m_transaction;
  std::atomic<tx::id> m_id;

public:
  explicit Tx(db::Context<hash_t> &ctx)
      : m_journal(ctx.journal()), m_transaction(), m_id(tx::START_ID) {
  }

  tx::Transaction begin() {
    return begin(tx::Settings());
  }

  tx::Transaction begin(tx::Settings &&s) {
    auto tid(m_id++);
    auto jid(m_journal.begin());
    return tx::Transaction{tid,jid, std::move(s)};
  }

  void commit(const Transaction &tx) {
    m_journal.commit(tx.jid);
    m_transaction.commit_await(tx);
  }

  void rollback(Transaction &) {
  }
};
}
#endif
