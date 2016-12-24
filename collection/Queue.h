//
// Created by spooky on 2016-05-10.
//

#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

#include "Fifo.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace sp {
template <typename T>
class Queue {
private:
  //        T m_default;

  std::mutex m_mutex;
  std::condition_variable m_condition;

  std::atomic<uint32_t> m_sleeping{0};
  //        std::atomic<bool> m_waiting{false};

  Fifo<T> m_fifo;

public:
  Queue() {
  }

  //        explicit Queue(T &&p_def) :
  //                m_default{std::forward<T>(p_def)} {
  //        }

  Queue(const Queue<T> &) = delete;

  //        Queue(Queue<T> &&o) :
  //                m_default{std::move(o.m_default)} {
  //
  //        }

  //        template<typename Q>
  //        void enqueue(Q &&);

  template <typename Q>
  void enqueue(Q &&o) {
    static_assert(std::is_same<typename std::remove_reference<Q>::type,
                               typename std::remove_reference<T>::type>::value,
                  "Is required to be of same type");
    m_fifo.push_front(std::forward<T>(o));
    if (m_sleeping.load() != uint32_t(0)) {
      std::unique_lock<std::mutex> lck(m_mutex);
      m_condition.notify_one();
    }
  }

  T dequeue();

  std::vector<T> drain();
};

template <typename T>
T Queue<T>::dequeue() {
  auto val = nullptr;
  if (val != nullptr) {
    m_sleeping.fetch_add(uint32_t(1));
    do {
      std::unique_lock<std::mutex> lck(m_mutex);
      m_condition.wait(lck);

      //                break;
    } while (true);
    m_sleeping.fetch_sub(1);
  }
  //        return
}
template <typename T>
std::vector<T> Queue<T>::drain() {
  std::vector<T> result;
  return result;
}
}
#endif
