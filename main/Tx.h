#ifndef _TX_TRANSACTION_TX
#define _TX_TRANSACTION_TX

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../shared/shared.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"

namespace {
template <typename Function>
class RAII {
private:
  Function release;

public:
  explicit RAII(Function f) : release(f) {
  }
  RAII(const RAII &) = delete;
  RAII(const RAII &&) = delete;
  ~RAII() {
    release();
  }
};
template <typename Function>
RAII<Function> raii(Function f) {
  return RAII<Function>(f);
}
}
namespace tx {

template <typename hash_t>
class Tx {
private:
  journal::Journals<hash_t> &m_journal;
  Transactionx<10> m_transaction;
  std::atomic<id> m_id;

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
    return tx::Transaction{tid, jid, std::move(s)};
  }

  void commit(const Transaction &tx) {
    m_journal.commit(tx.jid);
    // TODO order between journal commits and transaction commit
    // does have to be structured when it comes to udates and deletes
    // the order of which changes is applied should be the same
    // in the journal and transaction commits
    m_transaction.commit_await(tx);
  }

  void rollback(const Transaction &tx) {
    auto r1(raii([&] {
      //
      m_journal.rollback(tx.jid);
    }));

    //rollback probobly does not have the same constraints as commit.
    auto r2(raii([&] {
      //
      m_transaction.rollback(tx);
    }));
  }
};
}
#endif
