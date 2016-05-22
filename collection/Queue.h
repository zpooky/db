//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

namespace sp {

    template<typename T>
    class Queue {
    public:
        T m_default;
        explicit Queue(T && p_def) : m_default{std::move(p_def)} {

        }

        void push_front(T &&);

        T pop();
    };

    template<typename T>
    void Queue<T>::push_front(T &&o) {
    }

    template<typename T>
    T Queue<T>::pop() {
        return m_default;
    }

}
#endif
