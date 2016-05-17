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

            Node(T &&p_value) : value(std::move(p_value)) {

            }
        };

        std::atomic<Node *> m_head;
    public:
        List() : m_head(nullptr) {
        }

        template<typename Predicate, typename Supplier>
        T &find(Predicate p, Supplier s) {
            auto node = find(p);
            if (node == nullptr) {
                node = push_front_i(s());
            }
            return node->value;
        }

    private:
        Node *push_front_i(T &&v) {
            auto p = new Node(std::move(v));
            p->next = m_head;
            while (m_head.compare_exchange_weak(p->next, p)) { }
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
    };
}

#endif
