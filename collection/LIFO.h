//
// Created by spooky on 2016-07-20.
//

#ifndef PROJECT_LIFO_H
#define PROJECT_LIFO_H

#include <atomic>
#include <type_traits>

template<typename T>
class LIFO {
private:
    struct Entry;

    struct Entry {
        std::atomic<Entry *> previous{nullptr};
        T data;

        explicit Entry(T &&p_data) :
                data{std::forward<T>(p_data)} {

        }
    };

    std::atomic<Entry *> m_push{nullptr};
    std::atomic<Entry *> m_poll{nullptr};
public:

    template<typename D>
    void push_front(D &&data) {
        static_assert(
                std::is_same<typename std::remove_reference<D>::type, typename std::remove_reference<T>::type>::value,
                "Is required to be of same type");
        auto current = new Entry(std::forward<T>(data));
        auto next = m_push.load();

        while (!m_push.compare_exchange_strong(next, current));

        Entry *nil = nullptr;
        if (next) {
            bool b = next->previous.compare_exchange_strong(nil, current);
            assert(b);
        } else {
            bool b = m_poll.compare_exchange_strong(nil, current);
            assert(b);
        }


    }

    template<typename D>
    T pop(D &&def) {

        Entry *current = nullptr;
        do {
            current = m_poll.load();//redundant load just required for continue and initial value
            if (!current) {
                if (!m_push.load()) {
                    return std::forward<T>(def);//??
                } else {
                    continue;
                }
            }
        } while (!m_poll.compare_exchange_strong(current, current->previous));

        auto pinned = current;
        m_push.compare_exchange_strong(current, current->previous);

        T result(pinned->data);
        return result;
    }
};

#endif //PROJECT_LIFO_H
