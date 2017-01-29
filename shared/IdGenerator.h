#ifndef _DB_SHARED_ID_GENERATOR_H
#define _DB_SHARED_ID_GENERATOR_H
#include <atomic>
namespace db {
template <typename T>
class IdGenerator {
private:
  std::atomic<T> m_counter;

public:
  explicit IdGenerator(const T &current) : m_counter(current) {
  }

  IdGenerator(const IdGenerator &) = delete;
  IdGenerator(IdGenerator &&o) : m_counter(o.m_counter.load()) {
  }

  ~IdGenerator() {
  }

  T next();
};

template <typename T>
T IdGenerator<T>::next() {
  auto result = m_counter.load();
retry:
  if (!m_counter.compare_exchange_strong(result, result + 1)) {
    goto retry;
  }
  return result;
}
}

#endif
