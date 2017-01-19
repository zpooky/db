//
// Created by spooky on 2016-07-21.
//

#include "Barrier.h"

namespace sp {
void Barrier::await() {
  size_t expect = m_remaining.load();
  size_t replace;
  do {
    if (expect == size_t(0)) {
      /**
       * someone else will notify
       */
      return;
    }
    replace = expect-1;
  } while (!m_remaining.compare_exchange_strong(expect, replace));

  if (replace == size_t(0)) {
    // TODO if initial threads is 1 then this is not required
    std::unique_lock<std::mutex> lck(m_mutex);
    m_condition.notify_all();
  } else {
    std::unique_lock<std::mutex> lck(m_mutex);
    m_condition.wait(lck, [&] { //
      return m_remaining.load() == size_t(0);
    });
  }
}
}
