#ifndef _DB_TRANSACTION_LINE_ATOMICITY_H
#define _DB_TRANSACTION_LINE_ATOMICITY_H

#include "../collection/List.h"
#include "../collection/ReferenceCounter.h"
#include "transaction.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

namespace tx {
class Meta {
private:
public:
  Meta(){
  }

  template <typename Presents>
  explicit Meta(Presents &) {
  }

  template <typename Transactions>
  Meta(const Meta &, const Transactions &txs, const tx::Transaction &tx) {
  }

  Meta(const Meta &&) = delete;

private:
  // db::segment::id
};
}
namespace {
class Managed {
private:
  tx::Meta m_meta;
  sp::ReferenceCounter m_refc;

public:
  template <typename Presents>
  explicit Managed(Presents&p) : m_meta(p), m_refc() {
  }

  Managed():m_meta(),m_refc(){
  }

  // Internal
  void pin() {
    m_refc.increment();
  }
  // Internal
  void unpin() {
    m_refc.decrement();
  }
  // Internal
  using Tran = tx::Transaction;
  template <typename Transactions>
  void build(const tx::Meta &current, const Transactions &txs, const Tran &tx) {
    m_meta.~Meta();
    new (&m_meta) tx::Meta(current, txs, tx);
  }
  tx::Meta &meta() {
    return m_meta;
  }
  bool is_dead() const {
    return !m_refc;
  }
};
}

namespace tx {
class Current {
private:
  std::atomic<Managed *> m_current;

public:
  explicit Current(Managed *m) : m_current{m} {
  }
  void swap(Managed *replace) {
    auto current = m_current.load();
    replace->pin();
    assert(m_current.compare_exchange_strong(current, replace));
    current->unpin();
  }
  Managed &managed() {
    return *m_current.load();
  }
};
/**
 * Transactional cache [tx not yet commited]
 * - if tx creates a line it should be able to read it
 * - if tx removes a line it should not be able to read it
 *
 */
class Transactions {};

class CoalecedTransactions {
private:
  sp::con::Stack<tx::Transaction> m_stack;

public:
  CoalecedTransactions() : m_stack() {
  }
  void push_back(const Transaction &tx) {
    m_stack.push_front(Transaction(tx));
  }
  sp::Stack<tx::Transaction> drain() {
    return m_stack.drain();
  }
  bool contains(const tx::Transaction&) const {
    //TODO
    return true;
  }
};
struct ReleaseGuard {
private:
  std::mutex &m_mutex;

public:
  explicit ReleaseGuard(std::mutex &m) : m_mutex(m) {
  }
  ReleaseGuard(const ReleaseGuard &) = delete;
  ReleaseGuard(const ReleaseGuard &&) = delete;
  ~ReleaseGuard() {
    m_mutex.unlock();
  }
};
/**
 * Transaction atomicity [tx commit]
 *
 */
template <size_t areas>
class Transactionx {
private:
  using Areas_t = std::array<Managed, areas>;

  std::mutex m_commit_lock;
  std::mutex m_await_lock;
  std::condition_variable m_condition;

private:
  Areas_t m_areas;
  Current m_current;
  CoalecedTransactions m_transactions;

public:
  Transactionx()
      : m_commit_lock(), m_await_lock(), m_condition{}, m_areas{},
        m_current{&m_areas[0]}, m_transactions{} {
  }

  Transactionx(const Transactions &) = delete;

  void commit_await(const Transaction &tx) {
    if (m_commit_lock.try_lock()) {
      {
        ReleaseGuard guard(m_commit_lock);
        auto coaleced = m_transactions.drain();
        auto managed = find_reusable();
        Managed &current = m_current.managed();
        managed->build(current.meta(), coaleced, tx);
        m_current.swap(managed);
      }
      std::unique_lock<std::mutex> lck(m_await_lock);
      m_condition.notify_all();
    } else {
      m_transactions.push_back(tx);
      std::unique_lock<std::mutex> lck(m_await_lock);
      m_condition.wait(lck, [&] {
        return m_transactions.contains(tx);
      });
    }
  }

private:
  /* Current Ref Count should be pinned
   */
  Managed *find_reusable() {
    // TODO algorithm not loops
    for (auto &area : m_areas) {
      if (area.is_dead()) {
        return &area;
      }
    }
    assert(false);
    return nullptr;
  }
};

/**
 * Atomicity for individual lines [tx not yet commited]
 * - a tx remove should not be visible from other tx until commit.
 * it should only cache a removed line until commit.
 * To solve the concurrent read and removes from diffrent txs.
 */
class LineAtomicity {
public:
  // void removeIntention(const db::LineBuffer &b) {
  // }

  void readIntention() {
  }
};
/* # Commited Line
 * 1. When other is removeing line but not yet commited
 * 2. When other is conccurrently trying to remove
 *
 * # Not yet commited Line
 *
 */
class RemoveIntention {
private:
public:
  RemoveIntention() {
  }
};

class ReadIntention {
private:
public:
  ReadIntention() {
  }
};
}

#endif
