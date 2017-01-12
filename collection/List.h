//
// Created by spooky on 2016-05-12.
//

#ifndef _SP_LIST_H
#define _SP_LIST_H

#include <atomic>
#include <iterator>
#include <type_traits>
#include <utility>

namespace sp {
template <typename T>
class List {
private:
  struct Node {
    T value;
    Node *next;

    explicit Node(T &&p_value) : value(std::move(p_value)), next{nullptr} {
    }
  };
  class ForwardIterator : public std::iterator<std::forward_iterator_tag, T,
                                               std::ptrdiff_t, T *, T &> {
  private:
    Node *m_itr;

  public:
    explicit ForwardIterator(Node *it) : m_itr(it) {
    }

    ForwardIterator() : m_itr(nullptr) {
    }

    ForwardIterator &operator++() {
      m_itr = m_itr->next;
      return *this;
    }

    ForwardIterator operator++(int) {
      ForwardIterator it(*this);
      m_itr = m_itr->next;
      return it;
    }

    bool operator==(const ForwardIterator &o) const {
      return m_itr == o.m_itr;
    }

    bool operator!=(const ForwardIterator &o) const {
      return !operator==(o);
    }

    const T &operator*() const {
      return m_itr->value;
    }

    T &operator*() {
      return m_itr->value;
    }

    T *operator->() const {
      return &m_itr->value;
    }
  };

  std::atomic<Node *> m_head;
  std::atomic<unsigned long> m_cnt;

public:
  using element_type = T;
  using iterator = ForwardIterator;

  List() : m_head(nullptr), m_cnt(0) {
  }
  template <typename Collection>
  explicit List(Collection &&xs) : List() {
    using Entry_t = typename Collection::value_type;
    static_assert(std::is_same<Entry_t, T>::value, "");
    for (auto &e : xs) {
      push_front(std::move(e));
    }
  }

  List(const List<T> &) = delete;

  List(List<T> &&o) : m_head(o.m_head.load()), m_cnt(o.m_cnt.load()) {
    o.m_head.store(nullptr);
  }

  ~List() {
    Node *node = m_head.load();
    while (node != nullptr) {
      auto next = node->next;
      delete node;
      node = next;
    }
  }

  iterator begin() {
    return iterator{m_head};
  }

  iterator begin() const {
    return iterator{m_head};
  }

  iterator end() {
    return iterator{};
  }

  iterator end() const {
    return iterator{};
  }

private:
  Node *push_front_i(T &&v) {
    auto p = new Node(std::move(v));
    p->next = m_head.load();
    while (!m_head.compare_exchange_weak(p->next, p))
      ;
    m_cnt++;
    return p;
  }

  template <typename Predicate>
  Node *find(Predicate p) const {
    auto node = m_head.load();
    while (node != nullptr) {
      if (p(node->value)) {
        return node;
      }
      node = node->next;
    }
    return nullptr;
  }

public:
  template <typename Predicate, typename Supplier>
  T &find(Predicate p, Supplier s) {
    auto node = find(p);
    if (node == nullptr) {
      node = push_front_i(s());
    }
    return node->value;
  }

  template <typename Predicate>
  T *search(Predicate p) const {
    auto node = find(p);
    if (node) {
      return &node->value;
    }
    return nullptr;
  }

  void push_front(T &&data);
};

template <typename T>
void List<T>::push_front(T &&data) {
  push_front_i(std::move(data));
}
}

#endif
