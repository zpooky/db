//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include "Fifo.h"

namespace sp {
    template<typename T>
    class Queue {
    private:
//        T m_default;

        std::mutex m_mutex;
        std::condition_variable m_condition;

        std::atomic<uint32_t> m_sleeping{0};
//        std::atomic<bool> m_waiting{false};

        Fifo<T> m_fifo;

    public:

//        explicit Queue(T &&p_def) :
//                m_default{std::forward<T>(p_def)} {
//        }

        Queue(const Queue<T> &) = delete;

//        Queue(Queue<T> &&o) :
//                m_default{std::move(o.m_default)} {
//
//        }

        template<typename Q>
        void enqueue(Q &&);

        T dequeue();
    };

    template<typename T, typename Q>
    void Queue<T>::enqueue(Q &&o) {
        static_assert(std::is_same<typename std::remove_reference<Q>::type,
                typename std::remove_reference<T>::type>::value, "Is required to be of same type");
        m_fifo.push_front(std::forward<T>(o));
        if (m_sleeping.load() != 0u) {
            std::unique_lock<std::mutex> lck(m_mutex);
            m_condition.notify_one();
        }
    }

    template<typename T>
    T Queue<T>::dequeue() {
        auto val = nullptr;
        if (!val) {
            m_sleeping.fetch_add(1);
            do {
                std::unique_lock<std::mutex> lck(m_mutex);
                m_condition.wait(lck);

//                break;
            } while (true);
            m_sleeping.fetch_sub(1);
        }
//        return
    }

}
#endif
