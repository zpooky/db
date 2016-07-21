//
// Created by spooky on 2016-07-21.
//

#include "Barrier.h"

namespace sp {
    void Barrier::await() {
        size_t remaining = m_remaining.load();
        size_t expect;
        do {
            if (remaining == size_t(0)) {
                /**
                 * someone else will notify
                 */
                return;
            }
            expect = remaining;
            remaining -= size_t(1);
        } while (!m_remaining.compare_exchange_strong(expect, remaining));

        if (remaining == size_t(0)) {
            //TODO if initial threads is 1 then this is not required
            std::unique_lock<std::mutex> lck(m_mutex);
            m_condition.notify_all();
        } else {
            std::unique_lock<std::mutex> lck(m_mutex);
            //TODO load and compare if 0 then wait is not required
            m_condition.wait(lck, [&] {
                return m_remaining.load() == size_t(0);
            });
        }
    }

}