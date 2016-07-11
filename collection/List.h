//
// Created by spooky on 2016-05-12.
//

#ifndef _SP_LIST_H
#define _SP_LIST_H

#include <atomic>
#include <utility>

namespace sp {
    template<typename T>
    class List {
    private:
        struct Node {
            T value;
            Node *next;

            explicit Node(T &&p_value) :
                    value(std::move(p_value)),
                    next{nullptr} {
            }

            Node *operator++() {
                return next;
            }

            T &operator*() {
                return value;
            }

            const T &operator*() const {
                return value;
            }

            T *operator->() {
                return &value;
            }

            const T *operator->() const {
                return &value;
            }
        };

        std::atomic<Node *> m_head;
        std::atomic<unsigned long> m_cnt;
    public:
        using element_type = T;
        using iterator = Node *;
        using const_iterator = const iterator;

        List() :
                m_head(nullptr),
                m_cnt(0) {
        }

        List(const List<T> &) = delete;

        List(List<T> &&o) :
                m_head(o.m_head.load()),
                m_cnt(o.m_cnt.load()) {
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
            return m_head;
        }

        const_iterator begin() const {
            return m_head;
        }

        iterator end() {
            return nullptr;
        }

        const_iterator end() const {
            return nullptr;
        }

    private:
        Node *push_front_i(T &&v) {
            auto p = new Node(std::move(v));
            p->next = m_head;
            while (!m_head.compare_exchange_weak(p->next, p)) { }
            m_cnt++;
            return p;
        }

        template<typename Predicate>
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

        template<typename Predicate, typename Supplier>
        T &find(Predicate p, Supplier s) {
            auto node = find(p);
            if (node == nullptr) {
                node = push_front_i(s());
            }
            return node->value;
        }

        void push_front(T &&data);
    };

    template<typename T>
    void List<T>::push_front(T &&data) {
        push_front_i(std::move(data));
    }
}

#endif
