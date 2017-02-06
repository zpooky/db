#ifndef _TX_TRANSACTION_TX
#define _TX_TRANSACTION_TX

#include "../journal/Journals.h"
#include "../segment/Context.h"
#include "../shared/IdGenerator.h"
#include "../shared/shared.h"
#include "../transaction/LineAtomicity.h"
#include "../transaction/Settings.h"
#include "../transaction/transaction.h"

namespace {
template <typename Function>
class RAII {
private:
  Function release;
  bool present;

public:
  explicit RAII(Function f) : release(f), present(true) {
  }

  RAII(const RAII &) = delete;
  RAII(RAII &&o) : release(std::move(o.release)), present(false) {
    std::swap(present, o.present);
  }

  ~RAII() {
    if (present) {
      release();
      present = false;
    }
  }
};
template <typename Function>
RAII<Function> raii(Function f) {
  return RAII<Function>(f);
}
}
namespace tx {

class Tx {
private:
  journal::Journals &m_journal;
  Transactionx<10> m_transaction;
  db::IdGenerator<tx::id> m_counter;

public:
  explicit Tx(db::Context &ctx)
      : m_journal(ctx.journal()), m_transaction(), m_counter(tx::START_ID) {
  }

  tx::Transaction begin() {
    return begin(tx::Settings());
  }

  tx::Transaction begin(tx::Settings &&s) {
    auto tid(m_counter.next());
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

    // rollback probobly does not have the same constraints as commit.
    auto r2(raii([&] {
      //
      m_transaction.rollback(tx);
    }));
  }

  void introduce(CommitTree &&t) {
    m_transaction.introduce(std::move(t));
  }

  void release(const db::table::id &table) {
    m_transaction.release(table);
  }
};
}
#endif
