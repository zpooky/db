//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

#include <atomic>
#include <mutex>
#include <condition_variable>


namespace sp {


    template<typename T>
    class MPSCQueue {
    private:
        T m_default;

        std::mutex m_mutex;
        std::condition_variable m_condition;

        std::atomic<bool> m_sleeping{false};
//        std::atomic<bool> m_waiting{false};

    public:

        explicit MPSCQueue(T &&p_def) :
                m_default{std::forward<T>(p_def)} {
        }

        MPSCQueue(const MPSCQueue<T> &) = delete;

        MPSCQueue(MPSCQueue<T> &&o) :
                m_default{std::move(o.m_default)} {

        }

        void push_front(T &&);

        T pop();
    };

    template<typename T>
    void MPSCQueue<T>::push_front(T &&o) {
        std::unique_lock<std::mutex> lck(m_mutex);
        m_condition.notify_one();
    }

    template<typename T>
    T MPSCQueue<T>::pop() {
        std::unique_lock<std::mutex> lck(m_mutex);
        m_condition.wait(lck);
        return m_default;
    }

}
#endif
