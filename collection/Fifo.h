//
// Created by spooky on 2016-07-20.
//

#ifndef PROJECT_LIFO_H
#define PROJECT_LIFO_H

#include <atomic>
#include <utility>
#include <stdexcept>
#include <type_traits>

namespace sp {
/**
 * TODO not working
 */
template <typename T>
class Fifo {
private:
  struct Entry;

  struct Entry {
    std::atomic<Entry *> previous{nullptr};
    T data;

    explicit Entry(T &&p_data) : data{std::forward<T>(p_data)} {
    }
  };

  std::atomic<Entry *> m_push;
  std::atomic<Entry *> m_poll;

public:
  Fifo() : m_push{nullptr}, m_poll{nullptr} {
  }

  ~Fifo() {
  }

  template <typename D>
  void push_front(D &&data) {
    static_assert(std::is_same<typename std::remove_reference<D>::type,
                               typename std::remove_reference<T>::type>::value,
                  "Is required to be of same type");
    auto current = new Entry(std::forward<T>(data));
    auto next = m_push.load();

    while (!m_push.compare_exchange_strong(next, current))
      ;

    Entry *nil = nullptr;
    if (next) {
      bool b = next->previous.compare_exchange_strong(nil, current);
      if (!b) {
        throw std::runtime_error{"next true, b was false"};
      }
      //                assert(b);
    } else {
      bool b = m_poll.compare_exchange_strong(nil, current);
      if (!b) {
        throw std::runtime_error{"next false, b was false"};
      }
      //                assert(b);
    }
  }

  template <typename D>
  T pop(D &&def) {
    Entry *current = m_poll.load();
    do {
    redo:
      if (current == nullptr) {
        /**
         * if push head is empty
         * empty:
         *  the list is empty and return the supplied default
         * non empty:
         *  retry by loading from poll head.
         *  this is a edge case when a concurrent push is accuring into an empty
         * Fifo.
         *  initially the push head is set then the poll head. the pop is
         * performing in between those two events.
         */
        if (m_push.load() == nullptr) {
          return def;
        } else {
          current = m_poll.load();
          goto redo;
        }
      }
    } while (
        !m_poll.compare_exchange_strong(current, current->previous.load()));

    Entry *pinned = current;
    m_push.compare_exchange_strong(current, current->previous.load());

    T result(std::move(pinned->data));
    //            delete pinned;

    return result;
  }
};
}
#endif // PROJECT_LIFO_H
