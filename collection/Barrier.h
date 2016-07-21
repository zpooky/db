//
// Created by spooky on 2016-07-21.
//

#ifndef PROJECT_BARRIER_H
#define PROJECT_BARRIER_H

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace sp {
    class Barrier {
    private:
        std::mutex m_mutex;
        std::condition_variable m_condition;

        std::atomic<size_t> m_remaining;
    public:
        explicit Barrier(size_t threads) :
                m_remaining{threads} {
        }

        Barrier(const Barrier &) = delete;

    private:

    public:
        void await();
    };
}

#endif //PROJECT_BARRIER_H
