//
// Created by spooky on 2016-05-10.
//

#ifndef PROJECT_LIST_H
#define PROJECT_LIST_H

namespace sp {

    template<typename T>
    class Queue {
        void add(T &&);

        T get();
    };

    template<typename T>
    void Queue<T>::add(T &&o) {
    }

    template<typename T>
    T Queue<T>::get() {
        return {};
    }

}
#endif //PROJECT_LIST_H
