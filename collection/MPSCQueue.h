//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

namespace sp {


    template<typename T>
    class MPSCQueue {
    public:
        T m_default;

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
    }

    template<typename T>
    T MPSCQueue<T>::pop() {
        return m_default;
    }

}
#endif
