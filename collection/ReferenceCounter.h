#ifndef _DB_COLLECTION_REF_COUNTER
#define _DB_COLLECTION_REF_COUNTER

#include <atomic>

namespace {
struct Counter;
struct Counter {

  std::atomic<Counter *> next;
};
}
namespace sp {
class ReferenceCounter {
private:
  std::atomic<Counter *> m_cnt;

public:
  ReferenceCounter() : m_cnt{nullptr} {
  }
  ~ReferenceCounter() {
    Counter *current = m_cnt;
  begin:
    if (current) {
      auto tmp = current->next;
      delete current;
      current = tmp;
      goto begin;
    }
  }

public:
  Counter *new_managed() {
    auto cnt = new Counter;
    do {
      cnt->next = m_cnt.load();
    } while(!m_cnt.compare_exchange_strong(cnt->next, cnt);
    return cnt;
  }
};
}

#endif
