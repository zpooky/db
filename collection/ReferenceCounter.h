#ifndef _DB_COLLECTION_REF_COUNTER
#define _DB_COLLECTION_REF_COUNTER

#include <atomic>
#include <cstdint>

namespace sp {
class ReferenceCounter;


class ReferenceCounter {
private:
  std::atomic<std::uint64_t> m_cnt;

public:
  ReferenceCounter() : m_cnt{0} {
  }

  ReferenceCounter(const ReferenceCounter &&) = delete;
  ReferenceCounter(const ReferenceCounter &) = delete;

  operator bool() const & {
    return m_cnt.load() != 0;
  }
  operator bool() && = delete;

public:
  auto decrement() {
    auto current = m_cnt.load();
    do {
      if (current == 0) {
        break;
      }
    } while(!m_cnt.compare_exchange_strong(current, current-1));
    return current;
  }

  auto increment() {
    auto current = m_cnt.load();
    do {
      if (current == 0) {
        break;
      }
    } while(!m_cnt.compare_exchange_strong(current, current+1));
    return current;
  }
};

template<typename Counter>
struct CounterGuard {
private:
  Counter &m_cnt;
  bool m_valid;

public:
  explicit CounterGuard(Counter &cnt) : m_cnt(cnt), m_valid(false) {
    auto current = m_cnt.increment();
    m_valid = current > 0;
  }

  CounterGuard(const CounterGuard<Counter> &&) = delete;
  CounterGuard(const CounterGuard<Counter> &) = delete;

  operator bool() const & {
    return m_valid;
  }
  operator bool() & {
    return m_valid;
  }
  operator bool() && = delete;

  ~CounterGuard() {
    if (m_valid) {
      m_cnt.decrement();
    }
  }
};
}

#endif
