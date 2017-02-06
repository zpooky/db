#ifndef _DB_TRANSACTION_LINE_ATOMICITY_H
#define _DB_TRANSACTION_LINE_ATOMICITY_H

#include "../collection/List.h"
#include "../collection/ReferenceCounter.h"
#include "transaction.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include "CommitTree.h"

namespace {
class Meta {
private:
public:
  Meta() {
  }

  template <typename Presents>
  explicit Meta(const Presents &) {
  }

  Meta(const Meta &, const sp::Stack<tx::Transaction> &) {
  }

  Meta(const Meta &&) = delete;

private:
  // db::segment::id
};

class Managed {
private:
  Meta m_meta;
  sp::ReferenceCounter m_ref;

public:
  template <typename Presents>
  explicit Managed(const Presents &p) : m_meta(p), m_ref() {
  }

  Managed() : m_meta(), m_ref() {
  }

  // Internal
  void pin() {
    m_ref.increment();
  }
  // Internal
  void unpin() {
    m_ref.decrement();
  }
  // Internal
  void build(const Meta &current, const sp::Stack<tx::Transaction> &txs) {
    m_meta.~Meta();
    new (&m_meta) Meta(current, txs);
  }
  Meta &meta() {
    return m_meta;
  }
  bool is_dead() const {
    return !m_ref;
  }
};

class Current {
private:
  std::atomic<Managed *> m_current;

public:
  explicit Current(Managed *m) : m_current{m} {
  }
  void swap(Managed *replace) {
    assert(replace != nullptr);
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
  void push_back(const tx::Transaction &tx) {
    m_stack.push_front(tx::Transaction(tx));
  }
  sp::Stack<tx::Transaction> drain() {
    return m_stack.drain();
  }
  bool contains(const tx::Transaction &) const {
    // TODO
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
}
namespace tx {
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
  std::atomic<uint64_t> m_commit;

private:
  Areas_t m_areas;
  Current m_current;
  CoalecedTransactions m_transactions;

public:
  Transactionx()
      : m_commit_lock(), m_await_lock(), m_condition{}, m_commit(0), m_areas{},
        m_current{&m_areas[0]}, m_transactions{} {
  }

  Transactionx(const Transactions &) = delete;

  void commit_await(const Transaction &tx) {
    // TODO make work
    // commit id before tx is visible
    auto commit_id(m_commit.load());
    m_transactions.push_back(tx);
  retry:
    if (m_commit_lock.try_lock()) {
      {
        ReleaseGuard guard(m_commit_lock);
        auto coaleced = m_transactions.drain();
        ++m_commit;
        auto new_ = find_reusable();
        Managed &current = m_current.managed();
        new_->build(current.meta(), coaleced);
        m_current.swap(new_);
      }
      std::unique_lock<std::mutex> lck(m_await_lock);
      m_condition.notify_all();
    } else {
      {
        std::unique_lock<std::mutex> lck(m_await_lock);
        m_condition.wait(lck, [&] {
          // wait as long as it is the samme commit.
          // not a guarantee that the tx is commited
          return commit_id != m_commit.load();
        });
      }
      if (m_transactions.contains(tx)) {
        goto retry;
      }
    }
  }
  void rollback(const Transaction &) {
  }

  void introduce(tx::CommitTree&&) {
    std::unique_lock<std::mutex> lck(m_commit_lock);
  }

  void release(const db::table::id &){
    std::unique_lock<std::mutex> lck(m_commit_lock);
  }

private:
  /* m_current RefCount should be pinned
   * before calling this function
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
