//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

namespace sp {

    template<typename T>
    class Queue {
    public:
        void add(T &&);

        T pop();
    };

    template<typename T>
    void Queue<T>::add(T &&o) {
    }

    template<typename T>
    T Queue<T>::pop() {
        return {};
    }

}
#endif
