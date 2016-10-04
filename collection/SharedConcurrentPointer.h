//
// Created by spooky on 2016-10-04.
//

#ifndef PROJECT_SHAREDCONCURRENTPOINTER_H
#define PROJECT_SHAREDCONCURRENTPOINTER_H

#include <type_traits>
#include <atomic>

namespace sp {

    template<typename T>
    class SharedConcurrentPointer {
    public:
        using pointer_type = std::add_pointer<std::remove_pointer<T>::type>::type;
    private:
        pointer_type *m_data;
        std::atomic<uint64_t> m_references;
    public:
        explicit SharedConcurrentPointer(T *data) :
                m_data{data},
                m_references{1} {

        }

        explicit SharedConcurrentPointer(const SharedConcurrentPointer &) {

        }

        explicit SharedConcurrentPointer(SharedConcurrentPointer &&) {

        }

        ~SharedConcurentPointer() {
            auto current = m_references--;
            if (current == 0) {
                if (m_data) {
                    delete m_data;
                    m_data = nullptr;
                }
            }
        }

        SharedConcurrentPointer &operator=(const SharedConcurrentPointer &&) = delete;

        SharedConcurrentPointer &operator=(const SharedConcurrentPointer &) = delete;

        pointer_type &operator*() {
            return *m_data;
        }

        const pointer_type &operator*() const {
            return *m_data;
        }

        pointer_type *operator->() {
            return m_data;
        }

        const pointer_type *operator->() const {
            return m_data;
        }
    };
}

#endif //PROJECT_SHAREDCONCURRENTPOINTER_H
