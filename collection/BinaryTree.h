//
// Created by spooky on 2016-09-29.
//

#ifndef PROJECT_BINARYTREE_H
#define PROJECT_BINARYTREE_H

#include <memory>
#include <initializer_list>

namespace sp {
    template<typename T,
            typename Compare = std::less<T>>

    class BinaryTree {
    private:
        enum class Tag : bool {
            RED, BLACK
        };

        struct Node {
            T m_data;
            Tag m_tag{Tag::BLACK};
        private:
            Node *m_left;
            Node *m_right;
        public:
            explicit Node(T &&data) :
                    m_data(std::forward<T>(data)),
                    m_left(nullptr),
                    m_right(nullptr) {
            }

            ~Node() {
                if (m_left) {
                    delete m_left;
                    m_left = nullptr;
                }
                if (m_right) {
                    delete m_right;
                    m_right = nullptr;
                }
            }
        };

    public:
        using value_type = T;
        using allocator_type = std::allocator<Node>;
        using iterator = T *;
        using const_iterator = const iterator;
    private:
        allocator_type m_alloc;
        Node *m_root;

        template<typename Type, class... Args>
        Type *construct(Args &&... args) {
            Type *type = m_alloc.allocate(1);
            try {
                m_alloc.construct(type, std::forward<Args>(args)...);
            } catch (...) {
                m_alloc.deallocate(type, 1);
                throw;
            }
            return type;
        }

    public:
        explicit BinaryTree() :
                m_root(nullptr) {

        }

        ~BinaryTree() {
            if (m_root) {
                delete m_root;
            }
        }

        explicit BinaryTree(T &&data) :
                m_root(construct<Node>(std::forward<T>(data))) {
        }

        std::tuple<iterator, bool> insert(const value_type &val) {
            return std::make_tuple(nullptr, false);
        }

//        std::tuple<iterator, bool> insert(value_type &&val) {
//        }
//        void insert(std::initializer_list<value_type> il) {
//        }

        allocator_type get_allocator() const {
            return m_alloc;
        }

        iterator erase(const_iterator position) {
            return nullptr;
        }

        bool erase(const value_type &val) {
            return false;
        }
    };
}
#endif //PROJECT_BINARYTREE_H
